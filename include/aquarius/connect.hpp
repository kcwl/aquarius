#pragma once
#pragma warning(disable:4996)
#include <array>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <queue>
#include "router.hpp"
#include "police/socket_police.hpp"
#include "police/callback_police.hpp"
#include "detail/noncopyable.hpp"
#include "detail/deadline_timer.hpp"
#include "message/header.hpp"

namespace aquarius
{
	constexpr int heart_time_interval = 10;

	template<typename SocketPolice, typename CallbackPolice>
	class basic_connect
		: public std::enable_shared_from_this<basic_connect<SocketPolice, CallbackPolice>>
		, private detail::noncopyable
	{
		template<typename T>
		friend auto& SocketPolice::socket(std::shared_ptr<T>);

	public:
		template<typename... Args>
		explicit basic_connect(boost::asio::io_service& io_service, Args&&... args)
			: socket_(io_service, std::forward<Args>(args)...)
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
			return SocketPolice::socket(this->shared_from_this());
		}

		void start()
		{
			std::cout << "connect : " << size_++ << std::endl;

			SocketPolice::async_shake(this->shared_from_this());
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

		template<typename T>
		void queue_packet(T&& response)
		{
			ftstream fs;

			std::forward<T>(response)->to_bytes(fs);

			write_queue_.push(std::move(fs));
		}

		void set_buffer(ftstream buf)
		{
			read_buffer_ = buf;
		}

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
		void async_read()
		{
			socket_.async_read_some(boost::asio::buffer(read_buffer_.data(), read_buffer_.size()),
				[this, self = this->shared_from_this()](const boost::system::error_code& error, std::size_t bytes_transferred)
			{
				if (error)
				{
					return shut_down();
				}

				read_buffer_.commit(static_cast<int>(bytes_transferred));

				if (!handle_data())
				{
					return shut_down();
				}

				async_read();
			});
		}

		void shut_down()
		{
			if (socket_.is_open())
			{
				boost::system::error_code ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			}

			heart_timer_.cancel();

			socket_.close();

			cb_ptr_->invoke_callback<CallbackPolice::police_type::disconn>();
		}

		void establish_async_read()
		{
			cb_ptr_->invoke_callback<CallbackPolice::police_type::conn>();

			heart_timer_.expires_from_now(std::chrono::seconds(heart_time_interval));
			heart_timer_.async_wait(std::bind(&basic_connect::heart_deadline, this->shared_from_this()));

			async_read();
		}

		void heart_deadline()
		{
			if (heart_timer_.expires_at() <= detail::deadline_timer::traits_type::now())
			{
				queue_packet("");
			}

			heart_timer_.expires_from_now(std::chrono::seconds(heart_time_interval));
			heart_timer_.async_wait(std::bind(&basic_connect::heart_deadline, this->shared_from_this()));
		}
		public:
		bool handle_data()
		{
			while (true)
			{
				if (read_buffer_.active() == 0)
					break;

				if (read_buffer_.active() < sizeof(msg::common_header))
					return false;

				msg::common_header* header = (msg::common_header*)read_buffer_.read_pointer();

				auto str_proto_id = std::to_string(header->proto_id_);

				auto msg_ptr = invoke_helper<false>::invoke("msg_" + str_proto_id, std::ref(read_buffer_));

				if (msg_ptr == nullptr)
				{
					return false;
				}

				auto ctx_ptr = invoke_helper<true>::invoke("ctx_" + str_proto_id);

				if (ctx_ptr == nullptr)
					return false;

				ctx_ptr->attach_connect(this->shared_from_this());

				msg_ptr->accept(ctx_ptr); 
			}

			return true;
		}

		void set_callback(std::shared_ptr<CallbackPolice> cb_ptr)
		{
			cb_ptr_ = cb_ptr;
		}

	private:
		static inline int size_ = 0;

		SocketPolice::socket_t socket_;

		ftstream read_buffer_;

		detail::deadline_timer heart_timer_;

		boost::asio::ip::address remote_addr_;

		boost::asio::ip::port_type remote_port_;

		std::queue<ftstream> write_queue_;

		std::shared_ptr<CallbackPolice> cb_ptr_;
	};


	using connect = basic_connect<normal_socket_establish_police, callback_police>;
}
