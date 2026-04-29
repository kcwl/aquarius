#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_server.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/adaptor/raw_adaptor.hpp>
#include <aquarius/ip/adaptor/ssl_adaptor.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/handler_channel.hpp>
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

		struct callback
		{
			using func_t = std::function<void(flex_buffer)>;

			func_t func;

			bool complete;
		};

		template <typename T, typename Protocol>
		struct awaitable
		{
			std::size_t src;

			Protocol& self;

			T buffer;

			bool await_ready()
			{
				auto iter = self->buffers.find(src);
				if (iter != self->buffers.end())
				{
					if (iter->second.complete)
					{
						buffer = std::move(iter->second.buffer);
					}

					return iter->second.complete;
				}

				return true;
			}

			void await_suspend(std::coroutine_handle<void> h)
			{
				asio::co_spawn(
					co_await asio::this_coro::executor,
					[handle = std::move(h)]() -> asio::awaitable<void> { handle.resume(); }, asio::detached);
			}

			T await_resume()
			{
				return buffer;
			}

			awaitable get_return_objec()
			{
				return *this;
			}
		};

		struct raw_header
		{
			uint32_t length;
		};

		template <typename Session>
		auto accept(std::shared_ptr<Session> session_ptr) -> asio::awaitable<error_code>
		{
			error_code ec{};

			for (;;)
			{
				flex_buffer buffer{};

				ec = co_await recv(session_ptr, buffer);

				if (ec)
				{
					break;
				}

				auto router = binary_parse{}.from_datas<std::string>(buffer);

				XLOG_INFO() << "[accept] parse protocol router: " << router;

				asio::co_spawn(
					session_ptr->get_executor(),
					[&, r = std::move(router)] -> asio::awaitable<void>
					{
						auto result = co_await mpc_publish(std::move(r), buffer, session_ptr->uuid(), 0);

						if (!result.has_value())
						{
							XLOG_ERROR() << "[mpc_publish] publish error:" << ec.what();
							co_return;
						}

						auto res = co_await session_ptr->async_send(std::move(*result));

						if (res)
						{
							XLOG_ERROR() << "[tcp accept] async_send error:" << ec.what();
						}
					},
					asio::detached);
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

				ec = co_await recv(session_ptr, buffer);
				if (ec)
				{
					break;
				}

				std::size_t src = 0;

				if (!filling_buffer(src, std::move(buffer)))
				{
					auto router = binary_parse{}.from_datas<std::string>(buffer);

					XLOG_INFO() << "[query] parse protocol router: " << router;

					asio::co_spawn(
						session_ptr->get_executor(), [&, r = std::move(router)] -> asio::awaitable<void>
						{ co_await mpc_publish(std::move(r), buffer, session_ptr->uuid(), 0); }, asio::detached);
				}
			}

			if (ec != asio::error::eof)
			{
				XLOG_ERROR() << "[accept] error: " << ec.what();
			}

			co_return ec;
		}

		template <typename Func>
		auto wait(std::size_t src, Func&& f) -> awaitable<flex_buffer, tcp>
		{
			regist_resp_func(src, std::forward<Func>(f));

			co_return co_await awaitable<flex_buffer, tcp>(src, *this);
		}

	private:
		template <typename Session>
		auto recv(std::shared_ptr<Session> session_ptr, flex_buffer& buffer) -> asio::awaitable<error_code>
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

				ec = co_await session_ptr->async_read(buffer, header.length);
			}

			co_return ec;
		}

		bool filling_buffer(std::size_t src, flex_buffer buffer)
		{
			std::lock_guard lk(mutex_);

			auto iter = buffers_.find(src);

			if (iter == buffers_.end())
			{
				return false;
			}

			iter->second->func(std::move(buffer));

			iter->second->complete = true;

			return true;
		}

		void regist_resp_func(std::size_t src, const callback::func_t& func)
		{
			std::lock_guard lk(mutex_);

			auto iter = buffers_.find(src);
			if (iter != buffers_.end())
			{
				return;
			}

			auto ptr = std::make_shared<callback>();

			ptr->func = func;
		}

	private:
		std::mutex mutex_;

		std::map<std::size_t, std::shared_ptr<callback>> buffers_;
	};

	using tcp_session = basic_session<tcp, raw_adaptor>;
	using tcp_server = basic_server<tcp_session>;
	using tcp_client = basic_client<tcp_session>;

	using ssl_tcp_server = basic_server<basic_session<tcp, ssl_client_adaptor>>;
	using ssl_tcp_client = basic_client<basic_session<tcp, ssl_server_adaptor>>;
} // namespace aquarius