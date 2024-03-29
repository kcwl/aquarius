#pragma once
#include <aquarius/core/asio.hpp>
#include <aquarius/core/core.hpp>
#include <aquarius/core/logger.hpp>

namespace aquarius
{
	namespace impl
	{
		template <conn_mode Conn, ssl_mode SSL>
		class socket_adapter
		{
			constexpr static ssl_mode SSLMode = SSL;

			constexpr static conn_mode ConnMode = Conn;

		public:
			explicit socket_adapter(asio::socket_t socket, asio::ssl_context_t& ctx)
				: socket_(std::move(socket))
				, ssl_context_(ctx)
				, ssl_socket_(socket_, ssl_context_)
			{}

		public:
			auto& raw()
			{
				return socket_;
			}

			template <typename _Func>
			void start(_Func&& f)
			{
				if constexpr (SSLMode == ssl_mode::ssl)
				{
					ssl_socket_.async_handshake(static_cast<asio::ssl::stream_base::handshake_type>(ConnMode),
												[this, func = std::move(f)](const asio::error_code& ec)
												{
													if (ec)
													{
														XLOG_ERROR() << "handshake error at " << remote_address() << "("
																	 << remote_address_u() << "):" << remote_port()
																	 << "\t" << ec.message();

														return;
													}

													func();
												});
				}
				else
				{
					std::forward<_Func>(f)();
				}
			}

			std::string remote_address()
			{
				if (!socket_.is_open())
					return {};

				return socket_.remote_endpoint().address().to_string();
			}

			uint32_t remote_address_u()
			{
				if (!socket_.is_open())
					return {};

				return socket_.remote_endpoint().address().to_v4().to_uint();
			}

			uint16_t remote_port()
			{
				if (!socket_.is_open())
					return {};

				return socket_.remote_endpoint().port();
			}

			template <typename _Buffer, typename _Func>
			void async_read_some(const _Buffer& buffer, _Func&& f)
			{
				return socket_helper().async_read_some(buffer, std::forward<_Func>(f));
			}

			template <typename _Buffer, typename _Func>
			void async_write_some(const _Buffer& buffer, _Func&& f)
			{
				return socket_helper().async_write_some(buffer, std::forward<_Func>(f));
			}

			void shut_down()
			{
				if (!socket_.is_open())
					return;

				asio::error_code ec;

				socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);

				socket_.close(ec);

				ssl_socket_.shutdown(ec);
			}

			void close(bool shutdown)
			{
				asio::error_code ec;

				if (!socket_.is_open())
					return;

				shutdown ? socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec) : socket_.close(ec);
			}

			template <typename SettableSerialPortOption>
			void set_option(const SettableSerialPortOption& option, asio::error_code& ec)
			{
				socket_.set_option(option, ec);
			}

			void set_verify_mode(asio::ssl::verify_mode v)
			{
				if constexpr (SSLMode == ssl_mode::ssl)
				{
					return ssl_socket_.set_verify_mode(v);
				}
			}

		private:
			auto& socket_helper()
			{
				if constexpr (SSLMode == ssl_mode::ssl)
				{
					return ssl_socket_;
				}
				else
				{
					return socket_;
				}
			}

		private:
			asio::socket_t socket_;

			asio::ssl_context_t& ssl_context_;

			asio::ssl_socket_t ssl_socket_;
		};
	} // namespace impl
} // namespace aquarius