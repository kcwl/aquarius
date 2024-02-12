#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/deadline_timer.hpp>
#include <aquarius/detail/noncopyable.hpp>
#include <aquarius/invoke/invoke_session.hpp>
#include <aquarius/logger.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

namespace aquarius
{
	template <typename _Protocol, conn_mode Conn, ssl_mode SSL>
	class connect : public std::enable_shared_from_this<connect<_Protocol, Conn, SSL>>
	{
		using socket_t = boost::asio::ip::tcp::socket;

		using ssl_socket_t = boost::asio::ssl::stream<socket_t&>;

		using ssl_context_t = boost::asio::ssl::context;

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
			, uid_()
		{
			boost::uuids::random_generator_mt19937 generator{};

			uid_ = boost::uuids::hash_value(generator());
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

				ssl_socket_.async_handshake(static_cast<boost::asio::ssl::stream_base::handshake_type>(ConnMode),
											[this, self](const boost::system::error_code& ec)
											{
												if (ec)
												{
													XLOG(error) << "handshake error at " << remote_address() <<"("<<remote_address_u() << "):"
																<< remote_port() << "\t" << ec.message();

													return;
												}

												XLOG(info) << "handshake success at " << remote_address() << "(" << remote_address_u() << "):"
														   << remote_port() << ", async read establish";

												establish_async_read();
											});
			}
			else
			{
				XLOG(info) << "handshake success at " << remote_address() << ":" << remote_port()
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
				boost::asio::buffer(read_buffer_.rdata(), read_buffer_.active()),
				[this, self](const boost::system::error_code& ec, std::size_t bytes_transferred)
				{
					if (!socket_.is_open())
					{
						return;
					}

					if (ec)
					{
						if (ec != boost::asio::error::eof)
						{
							XLOG(error) << "on read some at " << remote_address() << ":" << remote_port()
										<< "\t"
										   " occur error : "
										<< ec.message();
						}

						return shut_down();
					}

					read_buffer_.commit(bytes_transferred);

					session_iovoke_helper::invoke(read_buffer_, uuid());

					async_read();
				});
		}

		template <typename _Func>
		void async_write(flex_buffer_t&& resp_buf, _Func&& f)
		{
			auto self(this->shared_from_this());

			sokcet_helper().async_write_some(
				boost::asio::buffer(resp_buf.wdata(), resp_buf.size()),
				[this, self, func = std::move(f)](const boost::system::error_code& ec,
												  [[maybe_unused]] std::size_t bytes_transferred)
				{
					if (!ec)
					{
						func();

						return;
					}

					XLOG(error) << "write error at " << remote_address() << "(" << remote_address_u() << "):"
								<< ":" << remote_port() << "\t" << ec.message();

					return shut_down();
				});
		}

		void shut_down()
		{
			callback_invoke_helper::invoke(uuid());

			boost::system::error_code ec;

			if (socket_.is_open())
			{
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

				socket_.close(ec);
			}

			ssl_socket_.shutdown(ec);
		}

		void close()
		{
			boost::system::error_code ec;

			if (socket_.is_open())
			{
				socket_.close(ec);
			}
		}

		std::size_t uuid() const
		{
			return uid_;
		}

		void set_verify_mode(boost::asio::ssl::verify_mode v)
		{
			ssl_socket_.set_verify_mode(v);
		}

	private:
		void establish_async_read()
		{
			auto session_ptr = std::make_shared<session<this_type>>(this->shared_from_this());

			if (!session_ptr)
			{
				XLOG(error) << "session create failed!";

				return;
			}

			session_manager::instance().push(session_ptr);

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

			socket_.set_option(boost::asio::socket_base::keep_alive(value), ec);

			XLOG(info) << "set keep alive :" << value;

			return !ec;
		}

		bool set_nodelay(bool enable)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::ip::tcp::no_delay(enable), ec);

			XLOG(info) << "set nodelay :" << enable;

			return !ec;
		}

		bool reuse_address(bool value)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::reuse_address(value), ec);

			XLOG(info) << "reuse address :" << value;

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
