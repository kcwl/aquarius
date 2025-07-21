#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/co_spawn.hpp>
#include <aquarius/context/client_router.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/context/handler_router.hpp>
#include <aquarius_protocol/tcp_response.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <array>

namespace aquarius
{
	namespace ip
	{
		template <typename Flow>
		class basic_tcp
		{
		public:
			using impl_type = boost::asio::ip::tcp;

			using socket = typename impl_type::socket;

			using no_delay = typename impl_type::no_delay;

			using acceptor = typename impl_type::acceptor;

			using resolver = typename impl_type::resolver;

			using endpoint_type = typename acceptor::endpoint_type;

			using session = basic_session<basic_tcp>;

			constexpr static bool is_server = Flow::is_server;

			using header = basic_tcp_header<is_server>;

		public:
			static endpoint_type make_v4_endpoint(uint16_t port)
			{
				return endpoint_type(impl_type::v4(), port);
			}

			static endpoint_type make_v6_endpoint(uint16_t port)
			{
				return endpoint_type(impl_type::v6(), port);
			}

		public:
			auto recv(std::shared_ptr<session> session_ptr, error_code& ec) -> awaitable<void>
			{
				constexpr auto header_length = header::impl_size;

				std::vector<char> header_buffer(header_length);

				ec = co_await session_ptr->async_read(header_buffer);
				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
						XLOG_ERROR() << "on read some occur error - " << ec.message();
					}
					session_ptr->shutdown();
					co_return;
				}

				header req_header{};
				req_header.consume(header_buffer);

				std::vector<char> body_buffer(req_header.length());

				ec = co_await session_ptr->async_read(body_buffer);
				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
						XLOG_ERROR() << "on read some occur error - " << ec.message();
					}
					session_ptr->shutdown();
					co_return;
				}

				Flow::recv(session_ptr, std::move(body_buffer), req_header);
			}
		};
	} // namespace ip
} // namespace aquarius