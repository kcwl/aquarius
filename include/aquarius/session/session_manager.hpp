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

			sessions_.emplace(session_ptr->uid(), session_ptr);

			return true;
		}

		bool erase(std::size_t id)
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(id);

			if (iter == sessions_.end())
				return true;

			iter->second->close();

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

		void report(std::size_t uid, int32_t port)
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(uid);

			if (iter == sessions_.end())
				return;

			if (!iter->second)
				return;

			iter->second->server_port(port);

			slave_sessions_.push_back(iter->second);
		}

		bool slave(const std::string& remote_ip, std::string& slave)
		{
			std::lock_guard lk(mutex_);

			for (auto& s : slave_sessions_)
			{
				if (!s)
					continue;

				hash_.add(s->remote_address(), s->server_port());
			}

			slave = hash_.get(remote_ip);

			return !slave.empty();
		}

		template <typename _Func>
		auto find_if(_Func&& f) -> std::vector<std::shared_ptr<xsession>>
		{
			std::vector<std::shared_ptr<xsession>> results{};

			for (auto& ptr : sessions_)
			{
				if (!std::forward<_Func>(f)(ptr.second))
					continue;

				results.push_back(ptr.second);
			}

			return results;
		}

		void clear()
		{
			std::lock_guard lk(mutex_);

			for (auto& session : sessions_)
			{
				session.second->close();
			}

			sessions_.clear();
		}

		std::size_t count()
		{
			std::lock_guard lk(mutex_);

			return sessions_.size();
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

		void send_someone(std::size_t uid, flex_buffer_t&& buffer)
		{
			std::lock_guard lk(mutex_);

			auto iter = sessions_.find(uid);

			if (iter == sessions_.end())
				return;

			if (!iter->second)
				return;

			iter->second->async_write(std::move(buffer));
		}

	private:
		std::unordered_map<std::size_t, std::shared_ptr<xsession>> sessions_;

		std::mutex mutex_;

		std::vector<std::shared_ptr<xsession>> slave_sessions_;

		std::mutex slave_mutex_;

		consistent_hash hash_;
	};

	inline std::shared_ptr<xsession> find_session(std::size_t id)
	{
		return session_manager::instance().find(id);
	}

	template <typename _Func>
	inline auto find_session_if(_Func&& f)
	{
		return session_manager::instance().find_if(std::forward<_Func>(f));
	}

	inline bool erase_session(std::size_t id)
	{
		return session_manager::instance().erase(id);
	}

	inline std::size_t count_session()
	{
		return session_manager::instance().count();
	}

	inline void clear_session()
	{
		return session_manager::instance().clear();
	}

	inline void report_session(std::size_t uid, int32_t port)
	{
		session_manager::instance().report(uid, port);
	}

	inline bool slave_session(const std::string& condition, std::string& slave)
	{
		return session_manager::instance().slave(condition, slave);
	}

} // namespace aquarius
