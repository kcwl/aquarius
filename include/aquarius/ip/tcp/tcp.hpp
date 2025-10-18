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

			detail::flex_buffer<char> buffer{};

			for (;;)
			{
				co_await recv(session_ptr, buffer, ec);

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

		template<typename Response, typename Session>
		auto query(std::shared_ptr<Session> session_ptr) -> awaitable<Response>
		{
			error_code ec{};

			detail::flex_buffer<char> buffer{};

			co_await recv_buffer(session_ptr, buffer, ec);

			Response resp{};

			if (!ec)
			{
				resp.consume(buffer, ec);
			}

			co_return resp;
		}

	private:
		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, detail::flex_buffer<char>& buffer, error_code& ec) -> awaitable<void>
		{
			co_await recv_buffer(session_ptr, buffer, ec);

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
				[buffer = std::move(buffer), session_ptr]() mutable -> awaitable<void>
				{
					serialize::binary_parse parse{};
					auto router = parse.from_datas<std::string_view>(buffer);
					
					tcp_router<Session>::get_mutable_instance().invoke(router, session_ptr, buffer);
					
					co_return;
				},
				detached);
		}

		template <typename Session>
		auto recv_buffer(std::shared_ptr<Session> session_ptr, detail::flex_buffer<char>& buffer, error_code& ec) -> awaitable<void>
		{
			uint32_t length = 0;

			constexpr auto len = sizeof(length);

			ec = co_await session_ptr->async_read((char*)&length, len);

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.message();
				}
				session_ptr->shutdown();
				co_return;
			}

			ec = co_await session_ptr->async_read(buffer, length);
		}
	};
} // namespace aquarius