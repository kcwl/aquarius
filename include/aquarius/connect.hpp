#pragma once
#pragma warning(disable:4996)
#include <array>
#include <memory>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include "common.hpp"
#include "detail/noncopyable.hpp"
#include "async_control.hpp"
#include <aquarius/qualify.hpp>

namespace aquarius
{
	namespace aqnet
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
				, body_length_(0)
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

			template<class T,std::size_t N>
			void async_write_some(const std::array<T,N>& buf)
			{
				socket_.async_write_some(boost::asio::buffer(buf.data(), buf.size()), [](const boost::system::error_code& ec,std::size_t bytes_transferred)
					{

					});
			}

		private:
			void async_read()
			{
				auto self = shared_from_this();
				boost::asio::async_read(socket_,boost::asio::buffer(buffer_.data(),static_cast<int>(MessageLength::max_message_head_length)),
					[this,self](const boost::system::error_code& error, std::size_t bytes_transferred)
					{
						if (error)
						{
							std::cout << error.value() << ":" << error.message() << std::endl;
							return;
						}

						//conn_buffer_.set_read_size(bytes_transferred);
						handle_read(error, bytes_transferred);
					});
			}

			void read_body()
			{
				auto self = this->shared_from_this();
				boost::asio::async_read(socket_, boost::asio::buffer(buffer_.data(), body_length_),
					[this,self](const boost::system::error_code& error, std::size_t bytes_transferred)
					{
						if(error)
						{
							std::cout << error.value() << ":" << error.message() << std::endl;
							return;
						}

						//control_ptr_->collect(std::move(buffer_), bytes_transferred);
						control_ptr_->complete(std::move(buffer_), bytes_transferred,1);

						async_read();
					});
			}

			void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
			{
				if (error || bytes_transferred < 20)
					return;

				//std::vector<std::byte> buf{};
				//std::for_each_n(buffer_.begin(), 20, [&](auto iter)
				//	{
				//		buf.push_back(iter);
				//	});

				//获取body长度
				constexpr uint32_t length = sizeof(uint32_t);
				//std::memcpy(&body_length_, reinterpret_cast<int*>(&buffer_.at(length*2)), length);
				body_length_ = buffer_.get<uint32_t>(length * 2);

				body_length_ > static_cast<int>(MessageLength::max_message_body_length) ? body_length_ = static_cast<int>(MessageLength::max_message_body_length) : 0;
			
				read_body();
			}

		private:
			static inline int size_ = 0;
#ifdef _SSL_SERVER
			ssl_socket_t socket_;
#else 
			tcp::socket socket_;
#endif
			//std::array<std::byte, 8192> buffer_;
			iostream buffer_;

			std::shared_ptr<async_control> control_ptr_;

			uint32_t body_length_;
		};
	}
}
