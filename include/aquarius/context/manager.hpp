#pragma once
#include <aquarius/connect/basic_connect.hpp>
#include <aquarius/core/manager.hpp>
#include <functional>
#include <mutex>
#include <set>

namespace aquarius
{
	class session_manager : public single_manager<session_manager, std::weak_ptr<basic_connect>>
	{
	public:
		bool push(std::weak_ptr<basic_connect> connect_ptr)
		{
			auto ptr = connect_ptr.lock();

			regist(ptr->uuid(), connect_ptr);

			return true;
		}

		void broadcast(flex_buffer_t&& buffer)
		{
			std::lock_guard lk(mutex_);

			for (auto& session : map_invokes_)
			{
				auto wptr = session.second;

				if (wptr.expired())
					continue;

				auto ptr = wptr.lock();

				ptr->send_packet(std::move(buffer));
			}
		}

		template <typename _Func>
		void broadcast(flex_buffer_t&& buffer, _Func&& f)
		{
			std::lock_guard lk(mutex_);

			for (auto& session : map_invokes_)
			{
				auto wptr = session.second;

				if (wptr.expired())
					continue;

				auto ptr = wptr.lock();

				if (!std::forward<_Func>(f)(ptr))
					continue;

				ptr->send_packet(std::forward<flex_buffer_t>(buffer));
			}
		}
	};
} // namespace aquarius
