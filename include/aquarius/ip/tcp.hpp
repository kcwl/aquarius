#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/basic_context.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/context/client_router.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/context/handler_router.hpp>
#include <aquarius/context/stream_context.hpp>
#include <aquarius/ip/lowyer_header.hpp>
#include <aquarius_protocol/tcp_response.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	class tcp
	{
	public:
		using impl_type = boost::asio::ip::tcp;

		using socket_type = typename impl_type::socket;

		using no_delay = typename impl_type::no_delay;

		using socket = typename impl_type::socket;

		using acceptor = typename impl_type::acceptor;

		using resolver = typename impl_type::resolver;

		using client_session = basic_session<false, tcp>;

		using server_session = basic_session<true, tcp>;

		using request_header = basic_tcp_header<true>;

		using response_header = basic_tcp_header<false>;

		using endpoint_type = typename acceptor::endpoint_type;

	public:
		static endpoint_type make_v4_endpoint(uint16_t port)
		{
			return endpoint_type(boost::asio::ip::tcp::v4(), port);
		}

		static endpoint_type make_v6_endpoint(uint16_t port)
		{
			return endpoint_type(boost::asio::ip::tcp::v6(), port);
		}

	public:
		template <typename Session>
		auto server_read(std::shared_ptr<Session> session, error_code& ec) -> awaitable<void>
		{
			static_assert(Session::is_server, "session must be for server");

			constexpr auto header_length = sizeof(request_header);
			char header_buffer[header_length] = { 0 };
			ec = co_await session->async_read(header_buffer);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session->shutdown();
				co_return;
			}

			request_header header{};

			std::memcpy((char*)&header, &header_buffer[0], header_length);

			std::vector<char> body_buffer(header.length());

			ec = co_await session->async_read(body_buffer);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session->shutdown();
				co_return;
			}

			co_spawn(
				session->get_executor(),
				[buffer = std::move(body_buffer), session, header]() -> awaitable<void>
				{
					static context::stream_context stream{};

					auto status =
						stream.invoke(session->get_executor(),
									  [buffer = std::move(buffer), session, header]
									  {
										  if (!context::detail::handler_router<Session>::get_mutable_instance().invoke(
												  header.rpc_id(), std::move(buffer), session, header))
										  {
											  context::detail::handler_router<Session>::get_mutable_instance().invoke(
												  __transfer_proto, std::move(buffer), session, header);
										  }
									  });

					if (status == std::future_status::timeout)
					{
						;
					}

					co_return;
				},
				detached);
		}

		template <typename Session>
		auto client_read(std::shared_ptr<Session> session, error_code& ec) -> awaitable<void>
		{
			static_assert(!Session::is_server, "session must be for client");

			constexpr auto header_length = sizeof(response_header);
			char header_buffer[header_length] = { 0 };
			ec = co_await session->async_read(header_buffer);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session->shutdown();
				co_return;
			}

			response_header header{};

			std::memcpy((char*)&header, &header_buffer[0], header_length);

			std::vector<char> body_buffer(header.length());

			ec = co_await session->async_read(body_buffer);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session->shutdown();
				co_return;
			}

			context::detail::client_router::get_mutable_instance().invoke(header.rpc_id(), std::move(body_buffer));
		}

		template <typename RPC, typename Session>
		auto client_send(std::shared_ptr<Session> session, typename RPC::request req, error_code& ec) -> awaitable<void>
		{
			static_assert(!Session::is_server, "session must be for client");

			std::vector<char> complete_buffer{};

			req.uuid(RPC::id);
			req.pack(complete_buffer);
			req.length(complete_buffer.size());

			co_await session->async_write_some(buffer(complete_buffer), ec);

			if (ec)
			{
				XLOG_ERROR() << "async write header is failed! maybe " << ec.message();
			}
		}
	};
} // namespace aquarius