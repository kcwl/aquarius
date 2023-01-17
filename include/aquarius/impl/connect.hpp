#pragma once
#include <aquarius/detail/deadline_timer.hpp>
#include <aquarius/detail/noncopyable.hpp>
#include <aquarius/impl/defines.hpp>
#include <aquarius/impl/flex_buffer.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <queue>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace impl
	{
		class session;

		using socket_t = boost::asio::ip::tcp::socket;

		using ssl_socket_t = boost::asio::ssl::stream<socket_t&>;

		using ssl_context_t = boost::asio::ssl::context;

		class connector
		{
		public:
			virtual void write(flex_buffer_t&&, std::chrono::steady_clock::duration) = 0;

			virtual void close() = 0;

			virtual flex_buffer_t& get_read_buffer() = 0;
		};

		template <typename _SocketType = void>
		class connect : private detail::noncopyable,
						public connector,
						public std::enable_shared_from_this<connect<_SocketType>>
		{
		public:
			explicit connect(boost::asio::io_service& io_service, int heart_time = heart_time_interval)
				: socket_(io_service)
				, ssl_context_(ssl_context_t::sslv23)
				, ssl_socket_(socket_, ssl_context_)
				, read_buffer_()
				, heart_timer_(io_service)
				, write_queue_()
				, heart_deadline_(heart_time)
				, last_operator_(true)
				, session_ptr_(nullptr)
				, conn_timer_(io_service)
			{
				init_context();
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
				return socket_.remote_endpoint().address().to_string();
			}

			uint16_t remote_port()
			{
				return socket_.remote_endpoint().port();
			}

			void set_delay(bool enable)
			{
				boost::system::error_code ec;
				socket_.set_option(boost::asio::ip::tcp::no_delay(enable), ec);
			}

			void write(flex_buffer_t&& resp_buf, std::chrono::steady_clock::duration timeout)
			{
				write_queue_.push(std::move(resp_buf));

				async_process_queue(timeout);
			}

			void async_read()
			{
				if (!socket_.is_open())
					return;

				read_buffer_.normalize();
				read_buffer_.ensure();

				if constexpr (std::same_as<_SocketType, ssl_socket>)
				{
					ssl_socket_.async_read_some(boost::asio::buffer(read_buffer_.wdata(), read_buffer_.active()),
												std::bind(&connect::read_handle, this->shared_from_this(),
														  std::placeholders::_1, std::placeholders::_2));
				}
				else
				{
					socket_.async_read_some(boost::asio::buffer(read_buffer_.wdata(), read_buffer_.active()),
											std::bind(&connect::read_handle, this->shared_from_this(),
													  std::placeholders::_1, std::placeholders::_2));
				}
			}

			void establish_async_read()
			{
				heart_timer_.expires_from_now(std::chrono::seconds(heart_deadline_.load()));
				heart_timer_.async_wait(std::bind(&connect::heart_deadline, this->shared_from_this()));

				on_start();

				async_read();
			}

			virtual flex_buffer_t& get_read_buffer() override
			{
				return read_buffer_;
			}

			void start()
			{
				if constexpr (std::same_as<_SocketType, ssl_socket>)
				{
					ssl_socket_.async_handshake(boost::asio::ssl::stream_base::server,
												[](const boost::system::error_code& ec)
												{
													if (ec)
														return;

													establish_async_read();
												});
				}
				else
				{
					establish_async_read();
				}
			}

			virtual void close() override
			{
				shut_down();

				on_close();
			}

			virtual void on_start()
			{}

			virtual void on_close()
			{}

		protected:
			void async_process_queue(std::chrono::steady_clock::duration dura)
			{
				if (write_queue_.empty())
					return;

				if (dura > 0s)
				{
					conn_timer_.expires_from_now(dura);
					conn_timer_.async_wait(
						[this, self = this->shared_from_this()](const boost::system::error_code& ec)
						{
							if (ec)
								return;

							shut_down();
						});
				}

				auto& buffer = write_queue_.front();

				if constexpr (std::same_as<_SocketType, ssl_socket>)
				{
					ssl_socket_.async_write_some(boost::asio::buffer(buffer.rdata(), buffer.size()),
												 std::bind(&connect::write_handle, this->shared_from_this(), dura,
														   std::placeholders::_1, std::placeholders::_2));
				}
				else
				{
					socket_.async_write_some(boost::asio::buffer(buffer.rdata(), buffer.size()),
											 std::bind(&connect::write_handle, this->shared_from_this(), dura,
													   std::placeholders::_1, std::placeholders::_2));
				}
			}

		private:
			void shut_down()
			{
				if (socket().is_open())
				{
					boost::system::error_code ec;
					socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				}

				heart_timer_.cancel();

				conn_timer_.cancel();

				socket().close();
			}

			void heart_deadline()
			{
				if (!last_operator_)
				{
					return shut_down();
				}

				if (heart_timer_.expires_at() <= detail::deadline_timer::traits_type::now())
				{
					// core::ping_request resp{};

					// queue_packet(std::move(resp));
				}

				last_operator_ ? last_operator_ = false : 0;

				heart_timer_.expires_from_now(std::chrono::seconds(heart_deadline_.load()));
				heart_timer_.async_wait(std::bind(&connect::heart_deadline, this->shared_from_this()));
			}

			void read_handle(const boost::system::error_code& ec, std::size_t bytes_transferred)
			{
				if (ec)
				{
					return shut_down();
				}

				read_buffer_.commit(static_cast<int>(bytes_transferred));

				!last_operator_ ? last_operator_ = true : 0;

				if (!session_ptr_)
				{
					auto session_ptr = std::make_shared<session>(this->shared_from_this());

					session_ptr_.swap(session_ptr);
				}

				session_ptr_->read();

				async_read();
			}

			void write_handle(std::chrono::steady_clock::duration dura, const boost::system::error_code& ec,
							  std::size_t bytes_transferred)
			{
				if (ec)
				{
					std::cout << ec.message() << std::endl;
					shut_down();
					return;
				}

				conn_timer_.cancel();

				write_queue_.pop();

				async_process_queue(dura);

				std::cout << "complete " << bytes_transferred << " bytes\n";
			}

			void init_context()
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

		private:
			socket_t socket_;

			ssl_context_t ssl_context_;

			ssl_socket_t ssl_socket_;

			flex_buffer_t read_buffer_;

			detail::deadline_timer heart_timer_;

			std::queue<flex_buffer_t> write_queue_;

			std::atomic_int heart_deadline_;

			bool last_operator_;

			std::shared_ptr<session> session_ptr_;

			detail::steady_timer conn_timer_;
		};
	} // namespace impl
} // namespace aquarius
