#pragma once
#pragma warning(disable:4996)
#include <array>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <queue>
#include "../router.hpp"
#include "../socket/socket.hpp"
#include "../core/noncopyable.hpp"
#include "../core/deadline_timer.hpp"
#include "../message/header.hpp"

namespace aquarius
{
	namespace conn
	{
		constexpr int heart_time_interval = 10;

		template<typename _Socket>
		class basic_connect
			: public std::enable_shared_from_this<basic_connect<_Socket>>
			, private core::noncopyable
		{
			template<typename _Socket>
			friend struct socket_traits;

		public:
			explicit basic_connect(boost::asio::io_service& io_service)
				: socket_(io_service)
				, read_buffer_()
				, heart_timer_(io_service)
				, remote_addr_()
				, remote_port_()
			{

			}

			virtual ~basic_connect()
			{
				shut_down();
			}

			auto& socket()
			{
				return socket_traits<_Socket>::socket(this->shared_from_this());
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

			template<typename _Ty>
			void queue_packet(_Ty&& response)
			{
				ftstream fs;

				std::forward<_Ty>(response).to_message(fs);

				write_queue_.push(std::move(fs));

				async_process_queue();
			}

			void set_buffer(ftstream buf)
			{
				read_buffer_ = buf;
			}

			void async_read()
			{
				socket_.async_read_some(boost::asio::buffer(read_buffer_.read_pointer(), read_buffer_.remain_size()),
					[this, self = this->shared_from_this()](const boost::system::error_code& error, std::size_t bytes_transferred)
				{
					if (error)
					{
						return shut_down();
					}

					read_buffer_.commit(static_cast<int>(bytes_transferred));

					if (!read_handle())
					{
						return shut_down();
					}

					async_read();
				});
			}

			auto& get_read_buffer()
			{
				return read_buffer_;
			}

			virtual void start() = 0;

			virtual void on_close() {}

			virtual bool read_handle() = 0;

		protected:
			void async_process_queue()
			{
				if (write_queue_.empty())
					return;

				auto& buffer = write_queue_.front();

				socket_.async_write_some(boost::asio::buffer(buffer.data(), buffer.size()), [this, self = this->shared_from_this()](const boost::system::error_code& ec, std::size_t bytes_transferred)
				{
					if (ec)
					{
						std::cout << ec.message() << std::endl;
						return;
					}

					if (write_queue_.front().size() == 0)
					{
						write_queue_.pop();
					}

					async_process_queue();

					std::cout << "complete " << bytes_transferred << "字节" << std::endl;
				});
			}

		private:
			void shut_down()
			{
				if (socket_.is_open())
				{
					boost::system::error_code ec;
					socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				}

				heart_timer_.cancel();

				socket_.close();

				on_close();
			}

			void establish_async_read()
			{
				heart_timer_.expires_from_now(std::chrono::seconds(heart_time_interval));
				heart_timer_.async_wait(std::bind(&basic_connect::heart_deadline, this->shared_from_this()));

				async_read();
			}

			void heart_deadline()
			{
				if (heart_timer_.expires_at() <= core::deadline_timer::traits_type::now())
				{
					queue_packet("");
				}

				heart_timer_.expires_from_now(std::chrono::seconds(heart_time_interval));
				heart_timer_.async_wait(std::bind(&basic_connect::heart_deadline, this->shared_from_this()));
			}

		private:
			static inline int size_ = 0;

			_Socket socket_;

			ftstream read_buffer_;

			core::deadline_timer heart_timer_;

			boost::asio::ip::address remote_addr_;

			boost::asio::ip::port_type remote_port_;

			std::queue<ftstream> write_queue_;
		};
	}

	using socket_connect = conn::basic_connect<boost::asio::ip::tcp::socket>;

#if ENABLE_SSL
	using ssl_connect = conn::basic_connect<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>;
#endif
}

