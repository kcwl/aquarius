#pragma once
#include <aquarius/elastic.hpp>
#include <aquarius/defines.hpp>
#include <aquarius/resolver.hpp>
#include <aquarius/invoke/invoke_message.hpp>
#include <aquarius/invoke/invoke_context.hpp>
#include <aquarius/invoke/router_session.hpp>

namespace aquarius
{
	struct invoke_session_helper
	{
		static read_handle_result process(flex_buffer_t& buffer, std::size_t uid)
		{
			auto session_ptr = find(uid);

			if (!session_ptr)
				return read_handle_result::unknown_error;

			uint32_t proto{};

			std::size_t total{};

			auto result = resolver<tcp>::template from_binay(buffer, proto, total);

			if (result != read_handle_result::ok)
				return result;

			auto request_ptr = invoke_message_helper::invoke(proto);

			auto context_ptr = invoke_context_helper::invoke(proto);

			if (!request_ptr)
			{
				buffer.consume(total);

				request_ptr = std::make_shared<basic_message>();
			}

			if (!context_ptr)
				context_ptr = std::make_shared<basic_context>();

			session_ptr->attach(proto, context_ptr);

			session_ptr->on_accept();

			result = request_ptr->accept(buffer, context_ptr, session_ptr);

			if (result == read_handle_result::ok)
				session_ptr->detach(proto);

			return result;
		}

		static bool push(std::shared_ptr<xsession> session_ptr)
		{
			if (!session_ptr)
				return false;

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

		template<typename _Response>
		static void send(std::size_t uid, _Response&& resp)
		{
			flex_buffer_t fs{};
			resp.to_binary(fs);

			router_session::instance().send(uid, std::move(fs));
		}

		static void timeout()
		{
			router_session::instance().timeout();
		}

		template<typename _Response>
		static void broadcast(_Response&& resp)
		{
			flex_buffer_t fs{};
			resp.to_binary(fs);

			router_session::instance().broadcast(std::move(fs));
		}

		template<typename _Response,typename _Pre>
		static void broadcast_if(_Response&& resp, _Pre&& func)
		{
			flex_buffer_t fs{};
			resp.to_binary(fs);

			router_session::instance().broadcast_if(std::move(fs), std::forward<_Pre>(func));
		}
	};
}