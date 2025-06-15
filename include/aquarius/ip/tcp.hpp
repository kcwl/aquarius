#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/context/auto_context.hpp>
#include <aquarius_protocol/tcp_header.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	class tcp : public boost::asio::ip::tcp
	{
	public:
		using base_type = boost::asio::ip::tcp;

		using socket_type = typename base_type::socket;

		using request_header = tcp_request_header;

		using response_header = tcp_response_header;

		struct local_header
		{
			uint32_t context_id;
			uint8_t mode;
			uint64_t length;
			uint32_t crc;
			int64_t timestamp;
		};

	public:
		using client_session = basic_session<false, tcp>;

		using server_session = basic_session<true, tcp>;

	public:
		template <typename Session>
		static auto read(std::shared_ptr<Session> session, error_code& ec) -> awaitable<void>
		{
			constexpr auto header_length = sizeof(local_header);
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

			local_header header{};

			std::memcpy((char*)&header, &header_buffer[0], header_length);

			std::vector<char> body_buffer(header.length);

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
				[buffer = std::move(body_buffer), session, header] -> awaitable<void>
				{
					context::auto_context_invoke<Session>::apply(header.mode, std::move(buffer), header.context_id,
																 session);

					co_return;
				},
				detached);
		}
	};
} // namespace aquarius