#pragma once
#include <aquarius/session.hpp>
#include <aquarius/system/singleton.hpp>
#include <aquarius/system/type_traits.hpp>
#include <functional>
#include <mutex>
#include <set>

namespace aquarius
{
	class basic_message;
}

namespace aquarius
{
	class router_session : public system::singleton<router_session>
	{
		template<typename _Func>
		struct invoker
		{
			static bool apply(const _Func& f, std::shared_ptr<aquarius::basic_message> msg)
			{
				using type = system::function_traits<_Func>::type;

				using element_t = typename type::element_type;

				auto ptr = std::dynamic_pointer_cast<element_t>(msg);

				if (!ptr)
					return false;

				f(ptr);

				return true;
			}
		};

	public:
		bool push(std::shared_ptr<xsession> session_ptr)
		{
			if (!session_ptr)
				return false;

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
				session.second->async_write(std::move(buffer));
			}
		}

		template <typename _Func>
		void broadcast_if(flex_buffer_t&& buffer, _Func&& f)
		{
			std::lock_guard lk(mutex_);

			for (auto& session : sessions_)
			{
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

		template<typename _Func>
		void regist(std::size_t id, _Func f)
		{
			std::lock_guard lk(callback_mutex_);

			callbacks_[id]= std::bind(&invoker<_Func>::apply, std::move(f), std::placeholders::_1);
		}

		bool apply(std::size_t id, std::shared_ptr<basic_message> message)
		{
			std::lock_guard lk(callback_mutex_);

			auto iter = callbacks_.find(id);

			if (iter == callbacks_.end())
				return false;

			return iter->second(message);
		}

	private:
		std::unordered_map<std::size_t, std::shared_ptr<xsession>> sessions_;

		std::mutex mutex_;

		std::map<std::size_t, std::function<bool(std::shared_ptr<basic_message>)>> callbacks_;

		std::mutex callback_mutex_;
	};
} // namespace aquarius
