#pragma once
#include <aquarius/context/manager.hpp>
#include <aquarius/context/impl/session.hpp>

namespace aquarius
{
	struct invoke_session_helper
	{
		static bool push(std::shared_ptr<basic_session> session_ptr)
		{
			return session_manager::instance().push(session_ptr);
		}

		static std::shared_ptr<basic_session> find(std::size_t uid)
		{
			return session_manager::instance().invoke(uid);
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
		static void broadcast(_Response&& resp)
		{
			error_code ec{};

			flex_buffer_t fs{};
			resp.to_binary(fs, ec);

			if (ec)
			{
				XLOG_ERROR() << "[Message] " << resp.uuid() << "to binary occur error.";

				return;
			}

			session_manager::instance().broadcast(std::move(fs));
		}

		template <typename _Response, typename _Pre>
		static void broadcast_if(_Response&& resp, _Pre&& func)
		{
			error_code ec{};

			flex_buffer_t fs{};
			resp.to_binary(fs, ec);

			if (ec)
			{
				XLOG_ERROR() << "[Message] " << resp.uuid() << "to binary occur error.";

				return;
			}

			session_manager::instance().broadcast_if(std::move(fs), std::forward<_Pre>(func));
		}
	};
} // namespace aquarius