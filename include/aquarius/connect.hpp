#pragma once
#pragma warning(disable:4996)
#include <array>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include "session.hpp"
#include "detail/callback.hpp"
#include "detail/noncopyable.hpp"
#include "tcp/protocol_enum.hpp"
#include "detail/deadline_timer.hpp"


namespace aquarius
{
	using btcp = boost::asio::ip::tcp;

	constexpr int heart_time_interval = 10;

#ifdef _SSL_SERVER
	using socket_t = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
#else
	using socket_t = btcp::socket;
#endif

	class connect
		: public detail::callback<connect>
		, public std::enable_shared_from_this<connect>
		, private detail::noncopyable
	{
	public:

		explicit connect(boost::asio::io_service& io_service
#ifdef _SSL_SERVER
			, boost::asio::ssl::context& context
#endif
		)
			: socket_(io_service
#ifdef _SSL_SERVER
				, context
#endif
			)

			, buffer_()
			, session_ptr_(new session())
			, heart_timer_(io_service)
		{
			auto self = shared_from_this();

			auto send_f = [self](streambuf&& buf)
			{
				self->async_write_some(std::move(buf));
			};


			session_ptr_->attach(send_f);
		}

		virtual ~connect()
		{
			shut_down();
		}

#ifdef _SSL_SERVER
		socket_t::lowest_layer_type& socket()
		{
			return socket_.lowest_layer();
		}
#else
		socket_t& socket()
		{
			return socket_;
		}
#endif

		void start()
		{
			std::cout << "connect : " << size_++ << std::endl;
#ifdef _SSL_SERVER
			socket_.async_handshake(boost::asio::ssl::stream_base::server,
				[this](const boost::system::error_code& error)
				{
					if (error)
					{
						std::cout << error.message() << std::endl;
						return;
					}
#endif
					establish();

					async_read();
#ifdef _SSL_SERVER
				});
#endif
		}

		void async_write_some(streambuf&& resp)
		{
			socket_.async_write_some(boost::asio::buffer(resp.data(), resp.size()), [](const boost::system::error_code& ec, std::size_t bytes_transferred)
				{
					if (ec)
					{
						std::cout << ec.message() << std::endl;
					}

					std::cout << "complete " << bytes_transferred << "字节" << std::endl;
				});
		}

		void set_connect_cb(connect_callback cb)
		{
			conn_cb_ = cb;
		}

		void set_disconnect_cb(disconnect_callback cb)
		{
			disconn_cb_ = cb;
		}

	private:
		void async_read()
		{
			auto self = shared_from_this();

			socket_.async_read_some(boost::asio::buffer(buffer_.data(), buffer_.max_size()),
				[this, self](const boost::system::error_code& error, std::size_t bytes_transferred) 
				{
					if (error)
					{
						shut_down();

						return;
					}

					buffer_.commit(static_cast<int>(bytes_transferred));

					session_ptr_->run(buffer_);
					
					async_read();
				});
		}

		void shut_down()
		{
			if (socket_.is_open())
			{
				boost::system::error_code ec;
				socket_.shutdown(btcp::socket::shutdown_both, ec);
			}

			heart_timer_.cancel();

			socket_.close();

			disconn_cb_ != nullptr ? disconn_cb_(shared_from_this()) : void();
		}

		void establish()
		{
			conn_cb_ != nullptr ? conn_cb_(shared_from_this()) : void();

			heart_timer_.expires_from_now(std::chrono::seconds(heart_time_interval));
			heart_timer_.async_wait(std::bind(&connect::heart_deadline, shared_from_this()));
		}

		void heart_deadline()
		{
			if (heart_timer_.expires_at() <= detail::deadline_timer::traits_type::now())
			{
				async_write_some({});
			}

			heart_timer_.expires_from_now(std::chrono::seconds(heart_time_interval));
			heart_timer_.async_wait(std::bind(&connect::heart_deadline, shared_from_this()));
		}

	private:
		static inline int size_ = 0;

		socket_t socket_;

		streambuf buffer_;

		detail::deadline_timer heart_timer_;

		std::shared_ptr<session> session_ptr_;
	};
}
