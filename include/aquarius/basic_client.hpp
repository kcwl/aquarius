#pragma once
#include <boost/asio.hpp>
#include <type_traits>
#include "detail/type_traits.hpp"
#include <aquarius/qualify.hpp>

namespace aquarius
{
	namespace aqnet
	{
		class basic_client
		{
		public:
			explicit basic_client(boost::asio::io_service& io_service,const boost::asio::ip::tcp::resolver::results_type& endpoints)
				: io_service_(io_service)
				, socket_(io_service)
				, buffer_()
			{
				do_connect(endpoints);
			}

			template<class... Args, class = std::enable_if_t<(sizeof...(Args) > 1)>>
			basic_client(boost::asio::io_service& io_service, Args&&... args)
				: io_service_(io_service)
				, socket_(io_service)
			{
				if constexpr (sizeof...(args) < 2)
					std::throw_with_nested(std::overflow_error("Usage: client <host> <port>"));

				auto endpoint_list = std::make_tuple(std::forward<Args>(args)...);

				auto host = std::get<0>(endpoint_list);
				auto port = std::get<1>(endpoint_list);

				if constexpr (!is_string<decltype(host)>::value || !is_string<decltype(port)>::value)
					throw std::overflow_error("Usage: client <host> <port> : type - string");

				boost::asio::ip::tcp::resolver resolver(io_service);

				auto endpoints = resolver.resolve(std::get<0>(endpoint_list), std::get<1>(endpoint_list));
				do_connect(endpoints);
			}

		public:
			template<class T, std::size_t N>
			void async_write(const std::array<T,N>& buf)
			{
				boost::asio::async_write(socket_,boost::asio::buffer(buf),
					[this](boost::system::error_code ec,std::size_t)
					{
						if(ec)
						{
							std::cout << ec.message() << std::endl;
						}
					});
			}

			//template<class T, std::size_t N>
			void async_write(const iostream& buf)
			{
				boost::asio::async_write(socket_, boost::asio::buffer(buf.str(),20),
					[this](boost::system::error_code ec, std::size_t)
					{
						if(ec)
						{
							std::cout << ec.message() << std::endl;
						}
					});
			}

			virtual int read_handler() = 0;

			void close()
			{
				socket_.close();
			}

		private:
			void do_connect(boost::asio::ip::tcp::resolver::results_type endpoints)
			{
				boost::asio::async_connect(socket_, endpoints,
					[this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
				{
					if (ec)
						return;

					do_read();
				});
			}

			void do_read()
			{
				boost::asio::async_read(socket_, boost::asio::buffer(buffer_),
					[this](boost::system::error_code ec,std::size_t bytes_transfer) 
					{
						if (ec)
							return;
					
						read_handler();

						do_read();
					});
			}

		private:
			boost::asio::io_service& io_service_;

			boost::asio::ip::tcp::socket socket_;

			//iostream conn_buffer_;
			std::array<std::byte, 8192> buffer_;
		};
	}
}