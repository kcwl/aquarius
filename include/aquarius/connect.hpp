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




namespace aquarius
{
	namespace net
	{
		using tcp =  boost::asio::ip::tcp;

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
				: socket_(io_service_,context)
	#else
				: socket_(io_service)
	#endif
				, buffer_()
				, control_ptr_(new async_control())
			{
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
					if (error)
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

			
			void async_write_some(const detail::streambuf& buf)
			{
				socket_.async_write_some(boost::asio::buffer(buf.data(),buf.size()), [](const boost::system::error_code& ec,std::size_t bytes_transferred)
					{

					});
			}

		private:
			void async_read()
			{
				auto self = shared_from_this();
				socket_.async_read_some(boost::asio::buffer(buffer_.data(),1024),
					[this, self] (const boost::system::error_code& error, std::size_t bytes_transferred){
						if (error)
						{
							std::cout << error.value() << ":" << error.message() << std::endl;
							return;
						}

						buffer_.commit(bytes_transferred);

						detail::streambuf os_buffer;

						control_ptr_->complete(buffer_,os_buffer);
					});
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
		};
	}
}
