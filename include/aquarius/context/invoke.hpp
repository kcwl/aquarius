#pragma once
#include <aquarius/context/manager.hpp>
#include <aquarius/core/error_code.hpp>

namespace aquarius
{
	struct invoke_session_helper
	{
		static bool push(std::shared_ptr<basic_connect> connect_ptr)
		{
			return session_manager::instance().push(connect_ptr);
		}

		static std::shared_ptr<basic_connect> find(std::size_t uid)
		{
			auto wptr = session_manager::instance().invoke(uid);

			if (wptr.expired())
				return nullptr;

			return wptr.lock();
		}

		static void erase(std::size_t uid)
		{
			return session_manager::instance().erase(uid);
		}

		static std::size_t size()
		{
			return session_manager::instance().size();
		}

		template <typename _Response>
		static bool broadcast(_Response&& resp)
		{
			flex_buffer_t fs{};

			aquarius::error_code ec{};

			resp.to_binary(fs, ec);

			session_manager::instance().broadcast(std::move(fs));

			return true;
		}

		template <typename _Response, typename _Pre>
		static bool broadcast_if(_Response&& resp, _Pre&& func)
		{
			flex_buffer_t fs{};

			aquarius::error_code ec{};

			resp.to_binary(fs, ec);

			session_manager::instance().broadcast_if(std::move(fs), std::forward<_Pre>(func));

			return true;
		}
	};
} // namespace aquarius