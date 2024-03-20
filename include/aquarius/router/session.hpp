#pragma once
#include <aquarius/router/impl/manager.hpp>
#include <aquarius/session/xsession.hpp>
#include <aquarius/system/type_traits.hpp>
#include <functional>
#include <mutex>
#include <set>

namespace aquarius
{
	namespace impl
	{
		class basic_message;
	}
} // namespace aquarius

namespace aquarius
{
	class router_session : public impl::single_manager<router_session, std::shared_ptr<xsession>>
	{
	public:
		bool push(std::shared_ptr<xsession> session_ptr)
		{
			if (!session_ptr)
				return false;

			regist(session_ptr->uuid(), session_ptr);

			return true;
		}

		void broadcast(flex_buffer_t&& buffer)
		{
			std::lock_guard lk(mutex_);

			for (auto& session : sessions_)
			{
				session.second->async_write(std::forward<flex_buffer_t>(buffer));
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
} // namespace aquarius
