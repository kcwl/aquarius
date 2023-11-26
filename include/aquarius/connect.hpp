#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/detail/deadline_timer.hpp>
#include <aquarius/detail/noncopyable.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/session/session.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <queue>

using namespace std::chrono_literals;

namespace aquarius
{
	template <typename _ProtoType, typename _SocketType>
	class connect : public std::enable_shared_from_this<connect<_ProtoType, _SocketType>>
	{
		using socket_t = boost::asio::ip::tcp::socket;

		using ssl_socket_t = boost::asio::ssl::stream<socket_t&>;

		using ssl_context_t = boost::asio::ssl::context;

		using this_type = connect<_ProtoType, _SocketType>;

	public:
		explicit connect(boost::asio::io_service& io_service,
						 std::chrono::steady_clock::duration dura = heart_time_interval)
			: socket_(io_service)
			, ssl_context_(ssl_context_t::sslv23)
			, ssl_socket_(socket_, ssl_context_)
			, read_buffer_()
			, connect_timer_(io_service)
			, connect_time_()
			, dura_(dura)
			, uid_()
			, session_ptr_(new session<this_type>())
		{
			init_ssl_context();

			boost::uuids::random_generator_mt19937 generator{};

			uid_ = boost::uuids::hash_value(generator());
		}

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
			if constexpr (std::same_as<_SocketType, ssl_socket>)
			{
				ssl_socket_.async_handshake(boost::asio::ssl::stream_base::server,
											[](const boost::system::error_code& ec)
											{
												if (ec)
												{
													XLOG(error) << "handshake error at " << remote_address() << ":"
																<< ec.message();

													return;
												}

												XLOG(info) << "handshake success at " << remote_address()
														   << ", async read establish";

												establish_async_read();
											});
			}
			else
			{
				XLOG(info) << "async read establish";

				establish_async_read();
			}
		}

		void async_read()
		{
			if (!socket_.is_open())
				return;

			read_buffer_.normalize();

			read_buffer_.ensure();

			auto self(this->shared_from_this());

			socket_helper().async_read_some(
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
							XLOG(error) << "on read some at " << remote_address() << " occur error : " << ec.message();
						}

						return shut_down();
					}

					read_buffer_.commit(bytes_transferred);

					auto result = session_ptr_->process(read_buffer_);

					if (result == read_handle_result::unknown_error || result == read_handle_result::unknown_proto)
					{
						XLOG(error) << "unknown proto at " << remote_address();

						return shut_down();
					}

					async_read();
				});
		}

		void async_write(flex_buffer_t&& resp_buf)
		{
			auto self(this->shared_from_this());

			socket_helper().async_write_some(
				boost::asio::buffer(resp_buf.wdata(), resp_buf.size()),
				[this, self](const boost::system::error_code& ec, [[maybe_unused]] std::size_t bytes_transferred)
				{
					if (!ec)
						return;

					XLOG(error) << "write error at " << remote_address() << ": " << ec.message();

					return shut_down();
				});
		}

		void shut_down()
		{
			if (socket_.is_open())
			{
				boost::system::error_code ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

				socket_.close();
			}

			connect_timer_.cancel();

			boost::system::error_code ec;
			ssl_socket_.shutdown(ec);
		}

		std::size_t uuid() const
		{
			return uid_;
		}

		std::chrono::milliseconds get_connect_duration()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() -
																		 connect_time_);
		}

		auto get_connect_time()
		{
			return connect_time_;
		}

		void set_sndbuf_size(int value) noexcept
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::send_buffer_size(value), ec);
		}

		int get_sndbuf_size() noexcept
		{
			boost::asio::socket_base::send_buffer_size option{};

			boost::system::error_code ec;

			socket_.get_option(option, ec);

			return option.value();
		}

		void set_rcvbuf_size(int value) noexcept
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::receive_buffer_size(value), ec);
		}

		int get_rcvbuf_size() noexcept
		{
			boost::asio::socket_base::receive_buffer_size option{};

			boost::system::error_code ec;

			socket_.get_option(option, ec);

			return option.value();
		}

		void keep_alive(bool value) noexcept
		{
			boost::system::error_code ec;

			socket_.set_option(boost::asio::socket_base::keep_alive(value), ec);
		}

		void set_nodelay(bool enable)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::ip::tcp::no_delay(enable), ec);
		}

		void reuse_address(bool value)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::reuse_address(value), ec);
		}

		void set_linger(bool enable, int timeout)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::linger(enable, timeout), ec);
		}

	private:
		void init_ssl_context()
		{
			if constexpr (std::same_as<_SocketType, ssl_socket>)
			{
				ssl_context_.set_options(ssl_context_t::default_workarounds | ssl_context_t::no_sslv2 |
										 ssl_context_t::single_dh_use);

				ssl_context_.set_password_callback([]() { return ""; });
				ssl_context_.use_certificate_chain_file("server.pem");
				ssl_context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
				ssl_context_.use_tmp_dh_file("dh4096.pem");
			}
		}

		void heart_deadline()
		{
			connect_timer_.expires_from_now(dura_);

			connect_timer_.async_wait(
				[this, self = this->shared_from_this()](const boost::system::error_code& ec)
				{
					if (ec == boost::asio::error::operation_aborted)
						return;

					if (!ec)
						return heart_deadline();

					shut_down();
				});
		}

		void establish_async_read()
		{
			connect_time_ = std::chrono::system_clock::now();

			session_ptr_ = std::make_shared<session<this_type>>(this->shared_from_this());

			session_manager::instance().push(session_ptr_);

			heart_deadline();

			async_read();
		}

		auto& socket_helper()
		{
			if constexpr (std::same_as<_SocketType, ssl_socket>)
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

		ssl_context_t ssl_context_;

		ssl_socket_t ssl_socket_;

		flex_buffer_t read_buffer_;

		detail::steady_timer connect_timer_;

		std::chrono::system_clock::time_point connect_time_;

		std::chrono::steady_clock::duration dura_;

		std::size_t uid_;

		std::shared_ptr<session<this_type>> session_ptr_;
	};
} // namespace aquarius
