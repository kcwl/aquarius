#pragma once
#include "../third_part/stduuid/include/uuid.h"

#include <aquarius/core/deadline_timer.hpp>
#include <aquarius/core/defines.hpp>
#include <aquarius/core/flex_buffer.hpp>
#include <aquarius/core/noncopyable.hpp>
#include <aquarius/server/basic_connector.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <queue>

using namespace std::chrono_literals;

namespace aquarius
{
	namespace srv
	{
		using socket_t = boost::asio::ip::tcp::socket;

		using ssl_socket_t = boost::asio::ssl::stream<socket_t&>;

		using ssl_context_t = boost::asio::ssl::context;

		template <typename _Ty, typename _SocketType>
		class connect : private core::noncopyable,
						public basic_connector,
						public std::enable_shared_from_this<connect<_Ty, _SocketType>>
		{
		public:
			explicit connect(boost::asio::io_service& io_service, std::chrono::steady_clock::duration dura)
				: socket_(io_service)
				, ssl_context_(ssl_context_t::sslv23)
				, ssl_socket_(socket_, ssl_context_)
				, read_buffer_()
				, write_queue_()
				, connect_timer_(io_service)
				, connect_time_()
				, dura_(dura)
				, uid_()
			{
				init_context();

				std::random_device rd{};

				auto seed_data = std::array<int, std::mt19937::state_size>{};
				std::generate(seed_data.begin(), seed_data.end(), std::ref(rd));
				std::seed_seq seq(seed_data.begin(), seed_data.end());
				std::mt19937 generator{ seq };
				
				uid_ = uuids::uuid_random_generator{ generator }();
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

			void write(core::flex_buffer_t&& resp_buf)
			{
				write_queue_.push(std::forward<core::flex_buffer_t>(resp_buf));

				async_process_queue();
			}

			void async_read()
			{
				if (!socket_.is_open())
					return;

				read_buffer_.normalize();
				read_buffer_.ensure();

				if constexpr (std::same_as<_SocketType, core::ssl_socket>)
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
				connect_time_ = std::chrono::system_clock::now();

				on_start();

				heart_deadline();

				async_read();
			}

			virtual core::flex_buffer_t& get_read_buffer() override
			{
				return read_buffer_;
			}

			void start()
			{
				if constexpr (std::same_as<_SocketType, core::ssl_socket>)
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

			virtual void shut_down()
			{
				if (socket_.is_open())
				{
					boost::system::error_code ec;
					socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				}

				connect_timer_.cancel();

				socket_.close();

				on_close();
			}

			virtual std::string uuid()
			{
				return uuids::to_string(uid_);
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

			void set_delay(bool enable)
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

		protected:
			void async_process_queue()
			{
				if (write_queue_.empty())
					return;

				auto& buffer = write_queue_.front();

				if constexpr (std::same_as<_SocketType, core::ssl_socket>)
				{
					ssl_socket_.async_write_some(boost::asio::buffer(buffer.rdata(), buffer.size()),
												 std::bind(&connect::write_handle, this->shared_from_this(),
														   std::placeholders::_1, std::placeholders::_2));
				}
				else
				{
					socket_.async_write_some(boost::asio::buffer(buffer.rdata(), buffer.size()),
											 std::bind(&connect::write_handle, this->shared_from_this(),
													   std::placeholders::_1, std::placeholders::_2));
				}
			}

			virtual core::read_handle_result read_handle_internal() = 0;

			virtual void on_start()
			{}

			virtual void on_close()
			{}

		private:
			void read_handle(const boost::system::error_code& ec, std::size_t bytes_transferred)
			{
				if (ec)
				{
					return shut_down();
				}

				read_buffer_.commit(static_cast<int>(bytes_transferred));

				auto res = read_handle_internal();

				if (res == core::read_handle_result::error)
				{
					return shut_down();
				}

				async_read();
			}

			void write_handle(const boost::system::error_code& ec, std::size_t bytes_transferred)
			{
				if (ec)
				{
					std::cout << ec.message() << std::endl;
					shut_down();
					return;
				}

				write_queue_.pop();

				async_process_queue();

				std::cout << "complete " << bytes_transferred << " bytes\n";
			}

			void init_context()
			{
				if constexpr (std::same_as<_SocketType, core::ssl_socket>)
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

		public:
			std::map<uint32_t, std::shared_ptr<ctx::context>> ctxs_;

			uuids::uuid uid_;

		private:
			socket_t socket_;

			ssl_context_t ssl_context_;

			ssl_socket_t ssl_socket_;

			core::flex_buffer_t read_buffer_;

			std::queue<core::flex_buffer_t> write_queue_;

			core::steady_timer connect_timer_;

			std::chrono::system_clock::time_point connect_time_;

			std::chrono::steady_clock::duration dura_;

			uuids::uuid uid_;
		};
	} // namespace srv
} // namespace aquarius
