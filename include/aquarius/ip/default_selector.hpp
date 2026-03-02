#pragma once
#include <aquarius/ip/http/http_method_define.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/module/channel_module.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	namespace ip
	{
		struct tcp_selector
		{
			tcp_selector(uint32_t seq)
				: seq_number(seq)
			{}

			template <typename Session>
			auto operator()(std::string_view router_, std::shared_ptr<Session> session_ptr, flex_buffer& buffer)
			{
				co_spawn(
					session_ptr->get_executor(),
					[&]() -> awaitable<void>
					{
						auto handler_ptr = co_await mpc_publish<Session>(router_);

						auto request_ptr = handler_ptr->request();

						request_ptr->consume_header(buffer);

						co_await handler_ptr->visit(session_ptr);
					},
					detached);
			};

			uint32_t seq_number;
		};

		struct http_selector
		{
			template <typename Handler, typename Session>
			auto operator()(std::shared_ptr<Handler> handler_ptr, std::shared_ptr<Session> session_ptr)
			{
				co_spawn(
					session_ptr->get_executor(),
					[&]() -> awaitable<void> { co_await handler_ptr->visit(session_ptr); }, detached);
			};
		};
	} // namespace ip
} // namespace aquarius