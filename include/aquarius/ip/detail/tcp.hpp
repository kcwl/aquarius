#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/context/client_router.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/context/handler_router.hpp>
#include <aquarius/ip/detail/tcp_base.hpp>
#include <aquarius_protocol/tcp_response.hpp>

namespace aquarius
{
	namespace ip
	{
		namespace detail
		{
			template <bool Server>
			class basic_tcp;

			template <>
			class basic_tcp<true> : public tcp_base
			{
				constexpr static bool is_server = true;

			public:
				using typename tcp_base::socket;

				using typename tcp_base::no_delay;

				using typename tcp_base::acceptor;

				using typename tcp_base::resolver;

				using typename tcp_base::endpoint_type;

				using session = basic_session<is_server, basic_tcp>;

				using header = basic_tcp_header<is_server>;

			public:
				auto recv(std::shared_ptr<session> session_ptr, error_code& ec) -> awaitable<void>
				{
					constexpr auto header_length = sizeof(header);
					char header_buffer[header_length] = { 0 };
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
					std::memcpy((char*)&req_header, &header_buffer[0], header_length);
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

					co_spawn(
						session_ptr->get_executor(),
						[buffer = std::move(body_buffer), session_ptr, h = std::move(req_header)]() mutable -> awaitable<void>
						{ 
							context::stream<basic_tcp>()(h.rpc_id(), session_ptr, std::move(buffer), h);
							co_return;
						}
					, detached);
				}
			};

			template <>
			class basic_tcp<false> : public tcp_base
			{
				constexpr static bool is_server = false;

			public:
				using typename tcp_base::resolver;
				using typename tcp_base::socket;
				using session = basic_session<is_server, basic_tcp>;
				using header = basic_tcp_header<is_server>;

			public:
				auto recv(std::shared_ptr<session> session_ptr, error_code& ec) -> awaitable<void>
				{
					constexpr auto header_length = sizeof(header);
					char header_buffer[header_length] = { 0 };
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
					header h{};
					std::memcpy((char*)&h, &header_buffer[0], header_length);
					std::vector<char> body_buffer(h.length());
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
					context::detail::client_router::get_mutable_instance().invoke(h.rpc_id(), std::move(body_buffer));
				}
				template <typename RPC>
				auto send(std::shared_ptr<session> session_ptr, typename RPC::request req, error_code& ec)
					-> awaitable<void>
				{
					std::vector<char> complete_buffer{};
					req.uuid(RPC::id);
					req.pack(complete_buffer);
					req.length(complete_buffer.size());
					co_await session_ptr->async_write_some(buffer(complete_buffer), ec);
					if (ec)
					{
						XLOG_ERROR() << "async write header is failed! maybe " << ec.message();
					}
				}
			};
		} // namespace detail
	} // namespace ip
} // namespace aquarius