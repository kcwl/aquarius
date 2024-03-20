#pragma once
#include <aquarius/logger.hpp>
#include <aquarius/system/asio.hpp>
#include <aquarius/system/deadline_timer.hpp>
#include <aquarius/system/defines.hpp>
#include <aquarius/system/uuid.hpp>
#include <aquarius/invoke/invoke_session.hpp>
#include <aquarius/session.hpp>

namespace aquarius
{
	template <typename _Protocol, conn_mode Conn, ssl_mode SSL>
	class connect : public std::enable_shared_from_this<connect<_Protocol, Conn, SSL>>
	{
		using socket_t = asio::ip::tcp::socket;

		using ssl_socket_t = ssl::stream<socket_t&>;

		using ssl_context_t = ssl::context;

		constexpr static conn_mode ConnMode = Conn;

		constexpr static ssl_mode SSLMode = SSL;

		using this_type = connect<_Protocol, ConnMode, SSLMode>;

	public:
		explicit connect(socket_t socket, ssl_context_t& basic_context,
						 std::chrono::steady_clock::duration dura = heart_time_interval)
			: socket_(std::move(socket))
			, ssl_socket_(socket_, basic_context)
			, read_buffer_()
			, dura_(dura)
			, uid_(uuid::invoke())
		{
			
		}

		connect(asio::io_service& io_service, ssl_context_t& basic_context,
				std::chrono::steady_clock::duration dura = heart_time_interval)
			: connect(std::move(asio::ip::tcp::socket(io_service)), basic_context, dura)
		{}

		virtual ~connect()
		{
			shut_down();
		}

		auto& socket()
		{
			return socket_;
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

		void start()
		{
			if constexpr (SSLMode == ssl_mode::ssl)
			{
				auto self = this->shared_from_this();

				ssl_socket_.async_handshake(static_cast<ssl::stream_base::handshake_type>(ConnMode),
											[this, self](const boost::system::error_code& ec)
											{
												if (ec)
												{
													XLOG_ERROR() << "handshake error at " << remote_address() << "("
																 << remote_address_u() << "):" << remote_port() << "\t"
																 << ec.message();

													return;
												}

												XLOG_INFO() << "handshake success at " << remote_address() << "("
															<< remote_address_u() << "):" << remote_port()
															<< ", async read establish";

												establish_async_read();
											});
			}
			else
			{
				XLOG_INFO() << "handshake success at " << remote_address() << ":" << remote_port()
							<< ", async read establish";

				establish_async_read();
			}
		}

		void async_read()
		{
			read_buffer_.normalize();

			read_buffer_.ensure();

			auto self(this->shared_from_this());

			sokcet_helper().async_read_some(
				asio::buffer(read_buffer_.rdata(), read_buffer_.active()),
				[this, self](const boost::system::error_code& ec, std::size_t bytes_transferred)
				{
					if (ec)
					{
						if (ec != asio::error::eof)
						{
							XLOG_ERROR() << "on read some at " << remote_address() << ":" << remote_port()
										 << "\t"
											" occur error : "
										 << ec.message();
						}

						return shut_down();
					}

					read_buffer_.commit(bytes_transferred);

					error_code ecr{};

					invoke_session_helper::process(read_buffer_, uuid(), ecr);

					async_read();
				});
		}

		template <typename _Func>
		void async_write(flex_buffer_t&& resp_buf, _Func&& f)
		{
			auto self(this->shared_from_this());

			sokcet_helper().async_write_some(
				asio::buffer(resp_buf.wdata(), resp_buf.size()),
				[this, self, func = std::move(f)](const boost::system::error_code& ec,
												  [[maybe_unused]] std::size_t bytes_transferred)
				{
					if (!ec)
					{
						func();

						return;
					}

					XLOG_ERROR() << "write error at " << remote_address() << "(" << remote_address_u() << "):"
								 << ":" << remote_port() << "\t" << ec.message();

					return shut_down();
				});
		}

		void shut_down()
		{	
			if (!socket_.is_open())
				return;

			boost::system::error_code ec;

			socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);

			socket_.close(ec);

			ssl_socket_.shutdown(ec);
		}

		void close(bool shutdown)
		{
			boost::system::error_code ec;

			if (!socket_.is_open())
				return;

			shutdown ? socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec) : socket_.close(ec);
		}

		std::size_t uuid() const
		{
			return uid_;
		}

		void set_verify_mode(ssl::verify_mode v)
		{
			ssl_socket_.set_verify_mode(v);
		}

	private:
		void establish_async_read()
		{
			auto session_ptr = std::make_shared<session<this_type>>(this->shared_from_this());

			invoke_session_helper::push(session_ptr);

			session_ptr->on_accept();

			keep_alive(true);

			if constexpr (ConnMode == conn_mode::server)
			{
				reuse_address(true);

				set_nodelay(true);
			}

			async_read();
		}

		bool keep_alive(bool value) noexcept
		{
			boost::system::error_code ec;

			socket_.set_option(asio::socket_base::keep_alive(value), ec);

			XLOG_INFO() << "set keep alive :" << value;

			return !ec;
		}

		bool set_nodelay(bool enable)
		{
			boost::system::error_code ec;
			socket_.set_option(asio::ip::tcp::no_delay(enable), ec);

			XLOG_INFO() << "set nodelay :" << enable;

			return !ec;
		}

		bool reuse_address(bool value)
		{
			boost::system::error_code ec;
			socket_.set_option(asio::socket_base::reuse_address(value), ec);

			XLOG_INFO() << "reuse address :" << value;

			return !ec;
		}

		auto& sokcet_helper()
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
		socket_t socket_;

		ssl_socket_t ssl_socket_;

		flex_buffer_t read_buffer_;

		std::chrono::steady_clock::duration dura_;

		std::size_t uid_;
	};
} // namespace aquarius
