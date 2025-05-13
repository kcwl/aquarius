#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/ssl_traits.hpp>
#include <aquarius/detail/ssl.hpp>

namespace aquarius
{
	namespace ssl
	{
		template <typename Protocol, typename Executor>
		class ssl_server
		{
		public:
			using socket_type = basic_stream_socket<Protocol, Executor>;

			using ssl_context_type = ssl::context&;

			using ssl_socket_type = ssl::stream<socket_type&>;

		public:
			ssl_server(socket_type& socket)
				: socket_(socket)
				, ssl_socket_(socket_, ssl_context_)
			{
			}

			~ssl_server()
			{
				error_code ec;
				ssl_socket_.shutdown(ec);
			}

		public:
			auto async_read_some(flex_buffer_t& buffer, error_code& ec) -> awaitable<std::size_t>
			{
				co_return co_await ssl_socket_->async_read_some(buffer(buffer.rdata(), buffer.active()),
					redirect_error(use_awaitable, ec));
			}

			auto async_write_some(flex_buffer_t buffer, error_code& ec) -> awaitable<std::size_t>
			{
				co_return co_await ssl_socket_->async_write_some(buffer(buffer.wdata(), buffer.size()),
					redirect_error(use_awaitable, ec));
			}

			auto start() -> awaitable<void>
			{
				error_code ec;

				co_await ssl_socket_->async_handshake(ssl::stream_base::handshake_type::server,
					redirect_error(use_awaitable, ec));
				if (ec)
				{
					co_return;
				}
			}

		private:
			socket_type& socket_;

			ssl_socket_type ssl_socket_;
		};

		template <typename Protocol, typename Executor>
		struct is_start<ssl_server<Protocol, Executor>> : std::true_type
		{
		};
	}
}