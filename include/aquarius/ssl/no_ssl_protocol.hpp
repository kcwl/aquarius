#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/error_code.hpp>

namespace aquarius
{
	namespace ssl
	{
		template <typename Protocol, typename Executor>
		class no_ssl_protocol
		{
		public:
			using socket_type = boost::asio::basic_stream_socket<Protocol, Executor>;

			using endpoint_type = boost::asio::ip::basic_endpoint<Protocol>;

		public:
			no_ssl_protocol(socket_type& socket)
				: socket_(socket)
			{}

		public:
			auto async_connect(const std::string& ip_addr, const std::string& port, error_code& ec) -> boost::asio::awaitable<void>
			{
				co_await socket_.async_connect(endpoint_type(boost::asio::ip::address::from_string(ip_addr),
															  static_cast<boost::asio::ip::port_type>(std::atoi(port.c_str()))),
												redirect_error(boost::asio::use_awaitable, ec));
			}

			auto async_read_some(flex_buffer_t& buffer, error_code& ec) -> boost::asio::awaitable<std::size_t>
			{
				co_return co_await socket_.async_read_some(boost::asio::buffer(buffer.rdata(), buffer.active()),
															redirect_error(boost::asio::use_awaitable, ec));
			}

			auto async_write_some(flex_buffer_t buffer, error_code& ec) -> boost::asio::awaitable<std::size_t>
			{
				if (!socket_.is_open())
				{
					//co_yield socket_.async_write_some(boost::asio::buffer(buffer.wdata(), buffer.size()),
					//											 redirect_error(boost::asio::use_awaitable, ec));
				}

				co_return co_await socket_.async_write_some(boost::asio::buffer(buffer.wdata(), buffer.size()),
															 redirect_error(boost::asio::use_awaitable, ec));
			}

			auto& get_implementation()
			{
				return socket_;
			}

		private:
			socket_type& socket_;
		};
	} // namespace ssl
} // namespace aquarius