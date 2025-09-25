#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <aquarius/serialize/binary.hpp>
#include <string_view>
#include <aquarius/ip/tcp/tcp_router.hpp>

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
			uint32_t length = 0;

			ec = co_await session_ptr->async_read((char*)&length, sizeof(length));
			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session_ptr->shutdown();
				co_return;
			}

			detail::flex_buffer<char> body_buffer{};

			length > body_buffer.remain() ? length = static_cast<uint32_t>(body_buffer.remain()) : 0;

			ec = co_await session_ptr->async_read(body_buffer, length);
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
				[buffer = std::move(body_buffer), session_ptr]() mutable -> awaitable<void>
				{
					virgo::binary_parse parse{};
					auto router = parse.from_datas<std::string_view>(buffer);

					tcp_router<Session>::get_mutable_instance().invoke(router, session_ptr, std::move(buffer));
					co_return;
				},
				detached);
		}
	};
} // namespace aquarius