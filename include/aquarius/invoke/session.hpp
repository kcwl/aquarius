#pragma once
#include <aquarius/context.hpp>
#include <aquarius/core/core.hpp>
#include <aquarius/core/elastic.hpp>
#include <aquarius/core/error_code.hpp>
#include <aquarius/invoke/context.hpp>
#include <aquarius/invoke/message.hpp>
#include <aquarius/message/message.hpp>
#include <aquarius/router/session.hpp>

namespace aquarius
{
	struct invoke_session_helper
	{
		static error_code process(flex_buffer_t& buffer, std::size_t uid, error_code& ec)
		{
			auto session_ptr = find(uid);

			uint32_t proto{};

			if (!elastic::from_binary(proto, buffer))
				return ec = system_errc::process_error;

			auto request_ptr = invoke_message_helper::invoke(proto);

			auto context_ptr = invoke_context_helper::invoke(proto);

			if (!context_ptr)
				context_ptr = std::make_shared<basic_context>();

			session_ptr->attach(proto, context_ptr);

			context_ptr->on_accept();

			if (!request_ptr)
			{
				request_ptr = std::make_shared<basic_message>();
			}

			auto future = std::async(std::launch::async, [&]
					   {
						   request_ptr->accept(buffer, context_ptr, session_ptr, ec);
					   });

			future.wait_for(timeout_dura);

			if (!ec)
				session_ptr->detach(proto);

			return ec;
		}

		static bool push(std::shared_ptr<basic_session> session_ptr)
		{
			return router_session::instance().push(session_ptr);
		}

		static std::shared_ptr<basic_session> find(std::size_t uid)
		{
			return router_session::instance().invoke(uid);
		}

		static void erase(std::size_t uid)
		{
			return router_session::instance().erase(uid);
		}

		template <typename _Response>
		static void broadcast(_Response&& resp)
		{
			error_code ec{};

			flex_buffer_t fs{};
			resp.to_binary(fs, ec);

			router_session::instance().broadcast(std::move(fs));
		}

		template <typename _Response, typename _Pre>
		static void broadcast_if(_Response&& resp, _Pre&& func)
		{
			error_code ec{};

			flex_buffer_t fs{};
			resp.to_binary(fs, ec);

			router_session::instance().broadcast_if(std::move(fs), std::forward<_Pre>(func));
		}
	};
} // namespace aquarius