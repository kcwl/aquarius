#pragma once
#pragma warning(disable:4996)
#include <array>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include "common.hpp"
#include "async_control.hpp"
#include "detail/noncopyable.hpp"
#include "detail/deadline_timer.hpp"
#include "detail/easybuffers/include/easybuffers.hpp"

namespace aquarius
{
	using tcp = boost::asio::ip::tcp;

#ifdef _SSL_SERVER
	using ssl_socket_t = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
#endif
	class connect
		: public std::enable_shared_from_this<connect>
		, private detail::noncopyable
	{
	public:
#ifdef _SSL_SERVER
		explicit connect(boost::asio::io_service& io_service, boost::asio::ssl::context& context)
#else
		explicit connect(boost::asio::io_service& io_service)
#endif
#ifdef _SSL_SERVER
			: socket_(io_service_, context)
#else
			:socket_(io_service)
#endif
			, buffer_()
			, control_ptr_(new async_control())
		{
		}

		virtual ~connect()
		{
			shut_down();
		}

#ifdef _SSL_SERVER
		ssl_socket_t::lowest_layer_type& socket()
		{
			return socket_.lowest_layer();
		}
#else
		boost::asio::ip::tcp::socket& socket()
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
										if(error)
										{
											std::cout << error.message() << std::endl;
											return;
										}
#endif
										async_read();
#ifdef _SSL_SERVER
									});
#endif
		}

		template<class Response>
		void async_write_some(Response&& resp)
		{
			easybuffers::ebstream stream{};
			resp.to_bytes(stream);

			socket_.async_write_some(boost::asio::buffer(stream.data(), stream.size()), [](const boost::system::error_code& ec, std::size_t bytes_transferred)
									 {
										 if(ec)
										 {
											 std::cout << ec.message() << std::endl;
										 }

										 std::cout << "complete " << bytes_transferred << "字节" << std::endl;
									 });
		}

	private:
		void async_read()
		{
			auto self = shared_from_this();

			socket_.async_read_some(boost::asio::buffer(buffer_.data(), buffer_.max_size()),
									[this, self](const boost::system::error_code& error, std::size_t bytes_transferred){
										if(error)
										{
											std::cout << error.value() << ":" << error.message() << std::endl;
											return;
										}

										buffer_.commit(bytes_transferred);

										control_ptr_->complete<uint32_t>(buffer_,self);

										async_read();
									});
		}

		void shut_down()
		{
			if(!state_)
				return;

			state_ = 0;

			if(socket_.is_open())
			{
				boost::system::error_code ec;
				socket_.shutdown(tcp::socket::shutdown_both, ec);
			}

			socket_.close();
		}

		void shut_down()
		{
			if(!state_)
				return;

			state_ = 0;

			if(socket_.is_open())
			{
				boost::system::error_code ec;
				socket_.shutdown(tcp::socket::shutdown_both, ec);
			}

			socket_.close();
		}


	private:
		static inline int size_ = 0;
#ifdef _SSL_SERVER
		ssl_socket_t socket_;
#else 
		tcp::socket socket_;
#endif
		detail::streambuf buffer_;

		std::shared_ptr<async_control> control_ptr_;

		int state_;
	};
}
