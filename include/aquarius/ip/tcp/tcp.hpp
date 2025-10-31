#pragma once
#include <boost/asio/ip/tcp.hpp>
#include <aquarius/serialize/binary.hpp>
#include <string_view>
#include <aquarius/ip/tcp/tcp_router.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/ip/concept.hpp>

namespace aquarius
{
	class tcp
	{
		using impl_type = boost::asio::ip::tcp;

	public:
		using socket = impl_type::socket;

		using endpoint = impl_type::endpoint;

		using acceptor = impl_type::acceptor;

		using resolver = impl_type::resolver;

		using no_delay = impl_type::no_delay;

	public:
		template <typename Session>
		requires(is_session<Session>)
		auto accept(std::shared_ptr<Session> session_ptr) -> awaitable<void>
		{
			flex_buffer buffer{};

			for (;;)
			{
				auto ec = co_await recv_buffer(session_ptr, buffer);

				if (ec)
				{
					if (ec != boost::asio::error::eof)
					{
						XLOG_ERROR() << "on read some occur error - " << ec.message();
					}

					session_ptr->shutdown();

					break;
				}

				co_spawn(
					session_ptr->get_executor(),
					[buffer = std::move(buffer), session_ptr]() mutable -> awaitable<void>
					{
						auto router = binary_parse{}.from_datas<std::string_view>(buffer);

						tcp_router<Session>::get_mutable_instance().invoke(router, session_ptr, buffer);

						co_return;
					},
					detached);
			}
		}

		template <typename Response, typename Session>
		requires(is_session<Session> && is_message<Response>)
		auto query(std::shared_ptr<Session> session_ptr) -> awaitable<Response>
		{
			flex_buffer buffer{};

			auto ec = co_await recv_buffer(session_ptr, buffer);

			Response resp{};

			if (!ec)
			{
				resp.consume(buffer);
			}

			co_return resp;
		}

	private:
		template <typename Session>
		auto recv_buffer(std::shared_ptr<Session> session_ptr, flex_buffer& buffer) -> awaitable<error_code>
		{
			uint32_t length = 0;

			constexpr auto len = sizeof(length);

			auto ec = co_await session_ptr->async_read((uint8_t*)&length, len);

			if (ec)
			{
				co_return ec;
			}

			co_return co_await session_ptr->async_read(buffer, length);
		}
	};

	template <>
	struct is_socket_type<boost::asio::ip::tcp::socket> : std::true_type
	{};
} // namespace aquarius
