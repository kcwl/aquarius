#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/basic_context.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/context/client_router.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/context/handler_router.hpp>
#include <aquarius/context/stream_context.hpp>
#include <aquarius/ip/lowyer_header.hpp>
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

		using typename base_type::no_delay;

	public:
		using client_session = basic_session<false, tcp>;

		using server_session = basic_session<true, tcp>;

	public:
		template <typename Session>
		static auto server_read(std::shared_ptr<Session> session, error_code& ec) -> awaitable<void>
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
					static context::stream_context stream{};

					auto status = stream.invoke(
						session->get_executor(),
						[buffer = std::move(buffer), session, header]
						{
							auto id = header.rpc_id;
							switch (static_cast<mode>(header.mode))
							{
							case mode::stream:
								break;
							case mode::transfer:
								{
									id = __transfer_proto;
									break;
								}
							default:
								break;
							}

							context::detail::handler_router<Session>::get_mutable_instance().invoke(
								id, std::move(buffer), session, header);
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
		static auto client_read(std::shared_ptr<Session> session, error_code& ec) -> awaitable<void>
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

			context::detail::client_router::get_mutable_instance().invoke(header.rpc_id, std::move(body_buffer));
		}

		template <typename Session, typename BufferSequence>
		static auto client_send(std::shared_ptr<Session> session, uint8_t mode, std::size_t rpc_id, BufferSequence buff,
								error_code& ec) -> awaitable<void>
		{
			local_header header{};
			header.mode = mode;
			header.length = buff.size();
			header.rpc_id = rpc_id;

			std::vector<char> complete_buffer(sizeof(local_header));

			std::memcpy(complete_buffer.data(), (char*)&header, sizeof(local_header));

			complete_buffer.insert(complete_buffer.end(), buff.begin(), buff.end());

			co_await session->async_write_some(buffer(complete_buffer), ec);

			if (ec)
			{
				XLOG_ERROR() << "async write header is failed! maybe " << ec.message();
			}
		}
	};
} // namespace aquarius