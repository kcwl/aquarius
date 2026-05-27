#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_context.hpp>
#include <aquarius/basic_server.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/adaptor/raw_adaptor.hpp>
#include <aquarius/ip/adaptor/ssl_adaptor.hpp>
#include <aquarius/ip/context_reg.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/serialize/binary.hpp>
#include <expected>
#include <type_traits>

namespace aquarius
{
	struct tcp
	{
		using socket = asio::ip::tcp::socket;

		using endpoint = asio::ip::tcp::endpoint;

		using acceptor = asio::ip::tcp::acceptor;

		using resolver = asio::ip::tcp::resolver;

		using no_delay = asio::ip::tcp::no_delay;

		using keep_alive = asio::socket_base::keep_alive;

		using session = basic_session<tcp, raw_adaptor>;

		using server = basic_server<session>;
		using client = basic_client<session>;

		using ssl_server = basic_server<basic_session<tcp, ssl_client_adaptor>>;
		using ssl_client = basic_client<basic_session<tcp, ssl_server_adaptor>>;

		using session_callback = std::function<asio::awaitable<error_code>(std::array<asio::const_buffer, 2>)>;

		template <typename Handler>
		using context = basic_context<Handler, tcp, uint32_t, session_callback>;

		struct raw_header
		{
			uint32_t length;
			uint32_t src;
		};

		template <typename Session>
		auto accept(std::shared_ptr<Session> session_ptr) -> asio::awaitable<error_code>
		{
			error_code ec{};

			for (;;)
			{
				flex_buffer buffer{};
				uint32_t src{};
				ec = co_await recv(session_ptr, buffer, src);

				if (ec)
				{
					break;
				}

				auto router = binary_parse{}.from_datas<std::string>(buffer);

				XLOG_INFO() << "[accept] parse protocol router: " << router;

				auto context = mpc_get_context(router);

				if (!context)
				{
					continue;
				}

				auto ptr = std::dynamic_pointer_cast<basic_protocol_context<tcp, uint32_t, session_callback>>(context);
				if (!ptr)
				{
					continue;
				}

				ec = co_await ptr->complete(this, session_ptr->uuid(), buffer, std::move(src),
												 [session_ptr]<typename ConstBufferSequence>(
													 ConstBufferSequence&& buffer) -> asio::awaitable<error_code>
												 { co_return co_await session_ptr->async_send(buffer); });

				if (ec)
				{
					XLOG_ERROR() << "[mpc_publish] publish error:" << ec.what();
				}
			}

			if (ec != asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		template <typename Session>
		auto query(std::shared_ptr<Session> session_ptr) -> asio::awaitable<error_code>
		{
			error_code ec{};

			for (;;)
			{
				flex_buffer buffer{};
				uint32_t src{};
				ec = co_await recv(session_ptr, buffer, src);
				if (ec)
				{
					break;
				}

				auto router = binary_parse{}.from_datas<std::string>(buffer);

				XLOG_INFO() << "[query] parse protocol router: " << router;

				if (!session_ptr->filling_buffer(src, buffer))
				{
					asio::co_spawn(
						session_ptr->get_executor(),
						[&, session_ptr, r = std::move(router), src]() mutable -> asio::awaitable<void>
						{
							auto context = mpc_get_context(r);

							if (!context)
							{
								co_return;
							}

							auto ptr =
								std::dynamic_pointer_cast<basic_protocol_context<tcp, uint32_t, session_callback>>(
									context);
							if (!ptr)
							{
								co_return;
							}

							[[maybe_unused]] auto result =
								co_await ptr->complete(this, session_ptr->uuid(), buffer, std::move(src), session_callback{});
						},
						asio::detached);
				}
			}

			if (ec != asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		template <typename Session, typename Request, typename Func>
		auto send_request(std::shared_ptr<Session> session_ptr, std::shared_ptr<Request> request, Func&& func,
						  error_code& ec) -> asio::awaitable<std::size_t>
		{
			raw_header header{};
			header.src = detail::uuid_generator()();

			flex_buffer buffer{};

			request->commit(buffer);

			header.length = static_cast<uint32_t>(buffer.size());

			session_ptr->regist_resp_func(header.src, func);

			std::array<asio::const_buffer, 2> buffers{ asio::buffer((char*)&header, sizeof(raw_header)),
													   buffer.data() };

			ec = co_await session_ptr->async_send(buffers);

			co_return header.src;
		}

		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, flex_buffer& buffer, uint32_t& src)
			-> asio::awaitable<error_code>
		{
			if (!session_ptr)
			{
				co_return asio::error::eof;
			}

			raw_header header{};

			static_assert(std::is_standard_layout_v<raw_header> && std::is_trivially_constructible_v<raw_header> &&
							  std::is_trivially_copyable_v<raw_header>,
						  "raw header is must be pod type");

			constexpr auto len = sizeof(raw_header);

			auto ec = co_await session_ptr->async_read(buffer, len);

			if (!ec)
			{
				buffer.sgetn((char*)&header, len);

				src = header.src;

				ec = co_await session_ptr->async_read(buffer, header.length);
			}

			co_return ec;
		}

		template <typename Handler, typename Func>
		auto handle_request(std::size_t session_id, flex_buffer& buffer, uint32_t src, Func&& func) -> asio::awaitable<error_code>
		{
			auto handler_ptr = std::make_shared<Handler>();

			auto ec = co_await handler_ptr->visit(session_id, buffer);

			if (ec)
			{
				co_return ec;
			}

			flex_buffer resp_buffer{};

			handler_ptr->response().commit(resp_buffer);

			raw_header header{ .length = static_cast<uint32_t>(resp_buffer.size()), .src = src };

			std::array<asio::const_buffer, 2> buffers = { asio::buffer((char*)&header, sizeof(header)),
														  resp_buffer.data() };

			auto res = co_await func(buffers);

			if (res)
			{
				XLOG_ERROR() << "[tcp accept] async_send error:" << ec.what();
			}

			co_return res;
		}
	};
} // namespace aquarius