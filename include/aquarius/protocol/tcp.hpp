#pragma once
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

		using header = virgo::tcp_header<true>;

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

	private:
		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, error_code& ec) -> awaitable<void>
		{
			constexpr auto header_length = header::impl_size;

			detail::flex_buffer<char> header_buffer;

			ec = co_await session_ptr->async_read(header_buffer, header_length);
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
			auto result = h.consume(header_buffer);

			if (!result.has_value())
				co_return;

			detail::flex_buffer<char> body_buffer{};
			ec = co_await session_ptr->async_read(body_buffer, h.length());
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
				[buffer = std::move(body_buffer), session_ptr, h = std::move(h)]() mutable -> awaitable<void>
				{
					detail::router<Session>::get_mutable_instance().invoke(std::to_string(h.rpc()), session_ptr, std::move(buffer));
					co_return;
				},
				detached);
		}
	};
} // namespace aquarius