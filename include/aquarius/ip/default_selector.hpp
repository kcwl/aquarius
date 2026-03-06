#pragma once
#include <aquarius/ip/http/http_method_define.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/module/channel_module.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_null_protocol.hpp>
#include <aquarius/virgo/http_version.hpp>

namespace aquarius
{
	struct tcp_selector
	{
		template <typename Session, typename Handler>
		auto operator()(std::shared_ptr<Session> session_ptr,std::shared_ptr<Handler> handler_ptr, flex_buffer& buffer)
		{
			co_spawn(
				session_ptr->get_executor(),
				[&]() -> awaitable<void>
				{
					auto request_ptr = handler_ptr->request();

					request_ptr->consume_header(buffer);

					error_code ec{};

					auto buf = co_await handler_ptr->visit(ec);

					if (!ec)
					{
						co_await session_ptr->async_send(std::move(buf));
					}
				},
				detached);
		};

		uint32_t seq_number;
	};

	struct http_selector
	{
		template <typename Handler, typename Session>
		auto operator()(std::shared_ptr<Session> session_ptr, std::shared_ptr<Handler> handler_ptr, flex_buffer&)
		{
			co_spawn(
				session_ptr->get_executor(), [&]() -> awaitable<void> 
				{ 
					error_code ec{};
					auto buffer = co_await handler_ptr->visit(ec); 

					if (!ec)
					{
						co_await session_ptr->async_send(std::move(buffer));
					}
				},
				detached);
		};
	};
} // namespace aquarius