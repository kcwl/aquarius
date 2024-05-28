#pragma once
#include <aquarius/connect/basic_connect.hpp>
#include <aquarius/core/manager.hpp>
#include <functional>
#include <mutex>
#include <set>

namespace aquarius
{
	class session_manager : public single_manager<session_manager, std::shared_ptr<basic_connect>>
	{
	public:
		bool push(std::shared_ptr<basic_connect> connect_ptr)
		{
			if (!connect_ptr)
				return false;

			regist(connect_ptr->uuid(), connect_ptr);

			return true;
		}

		std::size_t size() const
		{
			return this->map_invokes_.size();
		}

		void broadcast(flex_buffer_t&& buffer)
		{
			std::lock_guard lk(mutex_);

			for (auto& session : map_invokes_)
			{
				session.second->async_write(std::forward<flex_buffer_t>(buffer));
			}
		}

		template <typename _Func>
		void broadcast_if(flex_buffer_t&& buffer, _Func&& f)
		{
			std::lock_guard lk(mutex_);

			for (auto& session : map_invokes_)
			{
				if (!std::forward<_Func>(f)(session.second))
					continue;

				session.second->async_write(std::forward<flex_buffer_t>(buffer));
			}
		}
	};
} // namespace aquarius
