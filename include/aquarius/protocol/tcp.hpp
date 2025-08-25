#pragma once
#include <aquarius/flow/raw_buffer_flow.hpp>
#include <aquarius/handler.hpp>
#include <virgo.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	template <bool Server>
	class tcp
	{
	public:
		using socket = boost::asio::ip::tcp::socket;

		using endpoint = boost::asio::ip::tcp::endpoint;

		using acceptor = boost::asio::ip::tcp::acceptor;

		using resolver = boost::asio::ip::tcp::resolver;

		using header = virgo::tcp::detail::layer_header;

		using no_delay = boost::asio::ip::tcp::no_delay;

		using keep_alive = boost::asio::socket_base::keep_alive;

	public:
		template <typename Session>
		auto accept(std::shared_ptr<Session> session_ptr) -> awaitable<error_code>
		{
			error_code ec;

			for (;;)
			{
				co_await recv(session_ptr, ec);

				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
						XLOG_ERROR() << "on read some occur error - " << ec.what();
					}

					session_ptr->shutdown();

					co_return ec;
				}
			}
		}

	public:
		static endpoint make_v4_endpoint(uint16_t port)
		{
			return endpoint(boost::asio::ip::tcp::v4(), port);
		}

		static endpoint make_v6_endpoint(uint16_t port)
		{
			return endpoint(boost::asio::ip::tcp::v6(), port);
		}

	private:
		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, error_code& ec) -> awaitable<void>
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
			header h{};
			h.consume(header_buffer);
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

			std::string_view rpc_id = h.transfer() ? h.rpc() : rpc_transfer_flow::id;

			co_spawn(
				session_ptr->get_executor(),
				[buffer = std::move(body_buffer), session_ptr, h = std::move(h),
				 rpc_id]() mutable -> awaitable<void>
				{
					detail::router<Session>::get_mutable_instance().invoke(rpc_id, session_ptr, std::move(buffer),
																			   h);
					co_return;
				},
				detached);
		}
	};
} // namespace aquarius