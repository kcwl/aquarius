#pragma once
#include <aquarius/elastic.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/system/defines.hpp>
#include <aquarius/system/uuid.hpp>
#include <aquarius/invoke/invoke_resolver.hpp>
#include <aquarius/invoke/invoke_message.hpp>
#include <aquarius/invoke/invoke_context.hpp>
#include <aquarius/invoke/router_session.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>

namespace aquarius
{
	struct invoke_session_helper
	{
		static error_code process(flex_buffer_t& buffer, std::size_t uid, error_code& ec)
		{
			auto session_ptr = find(uid);

			uint32_t proto{};

			std::size_t total{};

			invoke_resolver_helper<tcp>::from_binary(buffer, proto, total, ec);

			if (ec)
				return ec;

			auto request_ptr = invoke_message_helper::invoke(proto);

			std::shared_ptr<basic_context> context_ptr = session_ptr->get(proto);

			if(!context_ptr)
				context_ptr = invoke_context_helper::invoke(proto);

			if(!context_ptr)
				context_ptr = std::make_shared<basic_context>();

			session_ptr->attach(proto, context_ptr);

			context_ptr->on_accept();

			if (!request_ptr)
			{
				buffer.consume(total);

				request_ptr = std::make_shared<basic_message>();
			}

			request_ptr->accept(buffer, context_ptr, session_ptr, ec);

			if (!ec)
				session_ptr->detach(proto);

			return ec;
		}

		static bool push(std::shared_ptr<xsession> session_ptr)
		{
			return router_session::instance().push(session_ptr);
		}

		static std::shared_ptr<xsession> find(std::size_t uid)
		{
			return router_session::instance().find(uid);
		}

		static void close(std::size_t uid)
		{
			auto session_ptr = router_session::instance().find(uid);

			if (!session_ptr)
				return;

			session_ptr->on_close();
		}

		static bool erase(std::size_t uid)
		{
			return router_session::instance().erase(uid);
		}

		template <typename _Response>
		static void send(std::size_t uid, _Response&& resp)
		{
			error_code ec{};

			flex_buffer_t fs{};
			resp.to_binary(fs, ec);

			router_session::instance().send(uid, std::move(fs));
		}

		static void timeout()
		{
			router_session::instance().timeout();
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