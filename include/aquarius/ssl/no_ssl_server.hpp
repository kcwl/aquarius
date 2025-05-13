#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/error.hpp>

namespace aquarius
{
	namespace ssl
	{
		template <typename Protocol, typename Executor>
		class no_ssl_server
		{
			using socket_type = basic_stream_socket<Protocol, Executor>;

		public:
			no_ssl_server(socket_type& socket)
				: socket_(socket)
			{
			}

		public:
			auto async_read_some(flex_buffer_t& buffer, error_code& ec) -> awaitable<std::size_t>
			{
				co_return co_await socket_->async_read_some(buffer(buffer.rdata(), buffer.active()),
					redirect_error(use_awaitable, ec));
			}

			auto async_write_some(flex_buffer_t buffer, error_code& ec) -> awaitable<std::size_t>
			{
				co_return co_await socket_->async_write_some(buffer(buffer.wdata(), buffer.size()),
					redirect_error(use_awaitable, ec));
			}

		private:
			socket_type& socket_;
		};
	}
}