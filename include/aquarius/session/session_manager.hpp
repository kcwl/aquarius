#pragma once
#include <aquarius/detail/singleton.hpp>
#include <aquarius/session/session.hpp>
#include <mutex>
#include <set>
#include <aquarius/detail/consistent_hash.hpp>

namespace aquarius
{
	class session_manager : public detail::singleton<session_manager>
	{
	public:
		bool push(std::shared_ptr<xsession> session_ptr)
		{
			std::lock_guard lk(mutex_);

			sessions_.emplace(session_ptr->uuid(), session_ptr);

			return true;
		}

		bool erase(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(id);

			if (iter == sessions_.end())
				return true;

			return sessions_.erase(id) != 0;
		}

		std::shared_ptr<xsession> find(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(id);

			if (iter == sessions_.end())
				return nullptr;

			return iter->second;
		}

		void broadcast(flex_buffer_t&& buffer)
		{
			std::lock_guard lk(mutex_);

			for (auto& session : sessions_)
			{
				if (!session.second)
					continue;

				session.second->async_write(std::move(buffer));
			}
		}

		template <typename _Func>
		void broadcast(flex_buffer_t&& buffer, _Func&& f)
		{
			std::lock_guard lk(mutex_);

			for (auto& session : sessions_)
			{
				if (!session.second)
					continue;

				if (!std::forward<_Func>(f)(session.second))
					continue;

				session.second->async_write(std::forward<flex_buffer_t>(buffer));
			}
		}

		bool send(std::size_t uid, flex_buffer_t&& buffer)
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(uid);

			if (iter == sessions_.end())
				return false;

			if (!iter->second)
				return false;

			iter->second->async_write(std::move(buffer));

			return true;
		}

		void timeout()
		{
			std::lock_guard lk(mutex_);

			for (auto& session : sessions_)
			{
				session.second->on_timeout();
			}
		}

	private:
		std::unordered_map<std::size_t, std::shared_ptr<xsession>> sessions_;

		std::mutex mutex_;
	};

	inline std::shared_ptr<xsession> find_session(std::size_t id)
	{
		return session_manager::instance().find(id);
	}

	inline bool erase_session(std::size_t id)
	{
		return session_manager::instance().erase(id);
	}

	inline bool send_session(std::size_t uid, flex_buffer_t&& buffer)
	{
		return session_manager::instance().send(uid, std::move(buffer));
	}
} // namespace aquarius
