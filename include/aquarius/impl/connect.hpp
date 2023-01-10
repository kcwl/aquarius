#pragma once
#include <aquarius/detail/deadline_timer.hpp>
#include <aquarius/detail/noncopyable.hpp>
#include <aquarius/impl/io.hpp>
#include <aquarius/impl/socket.hpp>
#include <array>
#include <memory>
#include <queue>
#include <vector>

namespace aquarius
{
	namespace impl
	{
		constexpr int heart_time_interval = 10;

		template <typename _Socket>
		class basic_connect : public std::enable_shared_from_this<basic_connect<_Socket>>, private core::noncopyable
		{
		public:
			explicit basic_connect(boost::asio::io_service& io_service, int heart_time)
				: socket_(io_service)
				, read_buffer_()
				, heart_timer_(io_service)
				, remote_addr_()
				, remote_port_()
				, heart_deadline_(heart_time)
				, last_operator_(true)
			{}

			virtual ~basic_connect()
			{
				shut_down();
			}

			auto& socket()
			{
				return socket_.socket();
			}

			std::string remote_address()
			{
				return socket().remote_endpoint().address().to_string();
			}

			uint16_t remote_port()
			{
				return socket().remote_endpoint().port();
			}

			void set_delay(bool enable)
			{
				boost::system::error_code ec;
				socket().set_option(boost::asio::ip::tcp::no_delay(enable), ec);
			}

			void push_back(flex_buffer_t&& resp_buf)
			{
				write_queue_.push(std::move(resp_buf));

				async_process_queue();
			}

			void async_read()
			{
				if (!socket_.is_open())
					return;

				read_buffer_.normalize();
				read_buffer_.ensure();

				socket().async_read_some(boost::asio::buffer(read_buffer_.wdata(), read_buffer_.active()),
										 [this, self = this->shared_from_this()](const boost::system::error_code& error,
																				 std::size_t bytes_transferred)
										 {
											 if (error)
											 {
												 return shut_down();
											 }

											 read_buffer_.commit(static_cast<int>(bytes_transferred));

											 !last_operator_ ? last_operator_ = true : 0;

											 if (!read())
												 return;

											 async_read();
										 });
			}

			void establish_async_read()
			{
				heart_timer_.expires_from_now(std::chrono::seconds(heart_deadline_.load()));
				heart_timer_.async_wait(std::bind(&basic_connect::heart_deadline, this->shared_from_this()));

				async_read();
			}

			auto& get_read_buffer()
			{
				return read_buffer_;
			}

			void start()
			{
				if constexpr (std::same_as<_Socket, ssl_socket>)
				{
					socket().async_handshake(boost::asio::ssl::stream_base::server,
											 [this](const boost::system::error_code& ec)
											 {
												 if (ec)
												 {
													 return shut_down();
												 }

												 on_start();

												 async_read();
											 });
				}
				else
				{
					on_start();

					async_read();
				}
			}

			virtual void on_start() = 0;

			virtual void on_close() = 0;

			virtual bool read() = 0;

		protected:
			void async_process_queue()
			{
				if (write_queue_.empty())
					return;

				auto& buffer = write_queue_.front();

				socket().async_write_some(boost::asio::buffer(buffer.rdata(), buffer.size()),
										  [this, self = this->shared_from_this(),
										   &buffer](const boost::system::error_code& ec, std::size_t bytes_transferred)
										  {
											  if (ec)
											  {
												  std::cout << ec.message() << std::endl;
												  shut_down();
												  return;
											  }

											  write_queue_.pop();

											  async_process_queue();

											  std::cout << "complete " << bytes_transferred << "字节" << std::endl;
										  });
			}

		private:
			void shut_down()
			{
				on_close();

				if (socket().is_open())
				{
					boost::system::error_code ec;
					socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				}

				heart_timer_.cancel();

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
				heart_timer_.async_wait(std::bind(&basic_connect::heart_deadline, this->shared_from_this()));
			}

		private:
			_Socket socket_;

			flex_buffer_t read_buffer_;

			detail::deadline_timer heart_timer_;

			boost::asio::ip::address remote_addr_;

			boost::asio::ip::port_type remote_port_;

			std::queue<flex_buffer_t> write_queue_;

			std::atomic_int heart_deadline_;

			bool last_operator_;
		};

		struct http
		{};

		template <typename _Socket, typename _Type = void>
		class connect : public basic_connect<_Socket>
		{
		public:
			explicit connect(boost::asio::io_service& io_service, int heart_time = heart_time_interval)
				: basic_connect<_Socket>(io_service, heart_time)
			{}

		public:
			virtual bool read() override
			{
				flex_buffer_t& read_buffer = this->get_read_buffer();

				if (read_buffer.size() < sizeof(uint32_t))
					return;

				uint32_t id{};

				elastic::binary_iarchive ia(read_buffer);
				ia >> id;

				auto res = invoke_helper::invoke(id, this->shared_from_this(), read_buffer);

				if (!res)
				{
					return false;
				}

				return true;
			}

			virtual void on_start() override
			{}

			virtual void on_close() override
			{}

			template <typename _Message>
			void write(_Message&& resp)
			{
				flex_buffer_t fs;

				std::forward<_Message>(resp).to_message(fs);

				this->push_back(std::move(fs));
			}
		};

		template <typename _Socket>
		class connect<_Socket, http> : public basic_connect<_Socket>
		{
		public:
			explicit connect(boost::asio::io_service& io_service, int heart_time = heart_time_interval)
				: basic_connect<_Socket>(io_service, heart_time)
			{}

		public:
			virtual void on_start() override
			{}

			virtual void on_close() override
			{}

			virtual bool read() override
			{}
		};

	} // namespace impl
} // namespace aquarius
