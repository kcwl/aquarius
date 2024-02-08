#pragma once
#include <aquarius/defines.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/deadline_timer.hpp>
#include <aquarius/detail/noncopyable.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/session/session_manager.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>

namespace aquarius
{
	template <typename _Protocol, bool HasServer>
	class ssl_connect : public std::enable_shared_from_this<ssl_connect<_Protocol, HasServer>>
	{
		using socket_t = boost::asio::ip::tcp::socket;

		using ssl_socket_t = boost::asio::ssl::stream<socket_t&>;

		using ssl_context_t = boost::asio::ssl::context;

		constexpr static bool has_server = HasServer;

		using this_type = ssl_connect<_Protocol, has_server>;

	public:
		explicit ssl_connect(boost::asio::io_service& io_service, ssl_context_t& context,
							 std::chrono::steady_clock::duration dura = heart_time_interval)
			: io_service_(io_service)
			, socket_(io_service_)
			, ssl_socket_(socket_, context)
			, read_buffer_()
			, dura_(dura)
			, uid_()
			, session_ptr_(new session<this_type>())
		{
			boost::uuids::random_generator_mt19937 generator{};

			uid_ = boost::uuids::hash_value(generator());
		}

		virtual ~ssl_connect()
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
			boost::asio::ssl::stream_base::handshake_type htype{};

			if constexpr (has_server)
			{
				htype = boost::asio::ssl::stream_base::server;
			}
			else
			{
				htype = boost::asio::ssl::stream_base::client;
			}

			ssl_socket_.async_handshake(htype,
										[this, self = this->shared_from_this()](const boost::system::error_code& ec)
										{
											if (ec)
											{
												XLOG(error) << has_server << "-"
															<< "handshake error at " << remote_address() << ":"
															<< remote_port() << "\t" << ec.message();

												return;
											}

											XLOG(info) << "handshake success at " << remote_address() << ":"
													   << remote_port() << ", async read establish";

											establish_async_read();
										});
		}

		void async_read()
		{
			if (!socket_.is_open())
				return;

			read_buffer_.normalize();

			read_buffer_.ensure();

			auto self(this->shared_from_this());

			ssl_socket_.async_read_some(
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

					if (!on_resolve())
					{
						auto result = session_ptr_->process(read_buffer_);

						if (result == read_handle_result::unknown_error || result == read_handle_result::unknown_proto)
						{
							XLOG(error) << "unknown proto at " << remote_address() << ":" << remote_port();

							return shut_down();
						}

						if (!on_read(result))
							return;
					}

					async_read();
				});
		}

		template <typename _Func>
		void async_write(flex_buffer_t&& resp_buf, _Func&& f)
		{
			auto self(this->shared_from_this());

			ssl_socket_.async_write_some(
				boost::asio::buffer(resp_buf.wdata(), resp_buf.size()),
				[this, self, func = std::move(f)](const boost::system::error_code& ec,
												  [[maybe_unused]] std::size_t bytes_transferred)
				{
					if (!ec)
					{
						func();

						return;
					}

					XLOG(error) << has_server << "-write error at " << remote_address() << ":" <<
						":" << remote_port() << "\t" << ec.message();

					return shut_down();
				});
		}

		void shut_down()
		{
			

			if (socket_.is_open())
			{
				boost::system::error_code ec;

				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

				socket_.close(ec);

				ssl_socket_.shutdown(ec);
			}
		}

		std::size_t uuid() const
		{
			return uid_;
		}

		void set_sndbuf_size(int value) noexcept
		{
			if (get_sndbuf_size() == value)
				return;

			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::send_buffer_size(value), ec);

			XLOG(info) << "set sndbuf size :" << value;
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
			if (get_rcvbuf_size() == value)
				return;

			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::receive_buffer_size(value), ec);

			XLOG(info) << "set rcvbuf size :" << value;
		}

		int get_rcvbuf_size() noexcept
		{
			boost::asio::socket_base::receive_buffer_size option{};

			boost::system::error_code ec;

			socket_.get_option(option, ec);

			return option.value();
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

		bool set_linger(bool enable, int timeout)
		{
			boost::system::error_code ec;
			socket_.set_option(boost::asio::socket_base::linger(enable, timeout), ec);

			XLOG(info) << "set linger :" << enable << ",timeout:" << timeout;

			return !ec;
		}

		void set_verify_mode(boost::asio::ssl::verify_mode v)
		{
			ssl_socket_.set_verify_mode(v);
		}

	public:
		virtual bool on_read(read_handle_result)
		{
			return false;
		}

		virtual bool on_resolve()
		{
			return false;
		}

	protected:
		[[nodiscard]] flex_buffer_t& buffer()
		{
			return read_buffer_;
		}

		[[nodiscard]] ssl_socket_t& ssl_socket()
		{
			return ssl_socket_;
		}

	private:
		void establish_async_read()
		{
			session_ptr_ = std::make_shared<session<this_type>>(this->shared_from_this());

			session_manager::instance().push(session_ptr_);

			keep_alive(true);

			if constexpr (has_server)
			{
				reuse_address(true);

				set_nodelay(true);
			}

			async_read();
		}

	private:
		boost::asio::io_service& io_service_;

		socket_t socket_;

		ssl_socket_t ssl_socket_;

		flex_buffer_t read_buffer_;

		std::chrono::steady_clock::duration dura_;

		std::size_t uid_;

		std::shared_ptr<session<this_type>> session_ptr_;
	};
} // namespace aquarius
