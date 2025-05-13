#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/ssl/ssl_traits.hpp>
#include <aquarius/ssl/ssl.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/detail/flex_buffer.hpp>

namespace aquarius
{
	namespace ssl
	{
		template <bool Server, typename Protocol, typename Executor>
		class ssl_protocol
		{
		public:
			using socket_type = boost::asio::basic_stream_socket<Protocol, Executor>;

			using ssl_context_type = boost::asio::ssl::context&;

			using ssl_socket_type = boost::asio::ssl::stream<socket_type&>;

			using endpoint_type = boost::asio::ip::basic_endpoint<Protocol>;

		public:
			ssl_protocol(socket_type& socket)
				: socket_(socket)
				, ssl_socket_(
					  socket_,
					  ssl_context_factory<boost::asio::ssl::context&, boost::asio::ssl::context::sslv23>::create())
			{}

			~ssl_protocol()
			{
				error_code ec;
				ssl_socket_.shutdown(ec);
			}

		public:
			auto async_connect(const std::string& ip_addr, const std::string& port, error_code& ec) -> boost::asio::awaitable<void>
			{
				co_await socket_->async_connect(endpoint_type(boost::asio::ip::address::from_string(ip_addr),
															  static_cast<ip::port_type>(std::atoi(port.c_str()))),
												redirect_error(boost::asio::use_awaitable, ec));
			}

			auto async_read_some(flex_buffer_t& buffer, error_code& ec) -> boost::asio::awaitable<std::size_t>
			{
				co_return co_await impl.ssl_socket->async_read_some(
					boost::asio::buffer(buffer.rdata(), buffer.active()),
					redirect_error(boost::asio::use_awaitable, ec));
			}

			auto async_write_some(flex_buffer_t buffer, error_code& ec) -> boost::asio::awaitable<std::size_t>
			{
				if (!socket_.is_open())
				{
					co_return co_yield ssl_socket_->async_write_some(
						boost::asio::buffer(buffer.wdata(), buffer.size()),
						redirect_error(boost::asio::use_awaitable, ec));
				}

				co_return co_await ssl_socket_->async_write_some(
					boost::asio::buffer(buffer.wdata(), buffer.size()),
					redirect_error(boost::asio::use_awaitable, ec));
			}

			auto start() -> boost::asio::awaitable<void>
			{
				error_code ec;

				if constexpr (Server)
				{
					co_await ssl_socket_->async_handshake(ssl::stream_base::handshake_type::server,
														  redirect_error(boost::asio::use_awaitable, ec));
				}
				else
				{
					co_await ssl_socket_->async_handshake(ssl::stream_base::handshake_type::client,
														  redirect_error(boost::asio::use_awaitable, ec));
				}
			}

		private:
			socket_type& socket_;

			ssl_socket_type ssl_socket_;
		};
	} // namespace ssl
} // namespace aquarius