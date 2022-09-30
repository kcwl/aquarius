#pragma once
#include <type_traits>
#include <boost/asio.hpp>
#include "stream/streambuf.hpp"

namespace aquarius
{
	class client
	{
	public:
		explicit client(boost::asio::io_service& io_service, const boost::asio::ip::tcp::resolver::results_type& endpoints)
			: io_service_(io_service)
			, socket_(io_service)
			, buffer_()
		{
			do_connect(endpoints);
		}

		template<class... _Args, class = std::enable_if_t<(sizeof...(_Args) > 1)>>
		client(boost::asio::io_service& io_service, _Args&&... args)
			: io_service_(io_service)
			, socket_(io_service)
		{
			if constexpr(sizeof...(args) < 2)
				std::throw_with_nested(std::overflow_error("Usage: client <host> <port>"));

			auto endpoint_list = std::make_tuple(std::forward<_Args>(args)...);

			auto host = std::get<0>(endpoint_list);
			auto port = std::get<1>(endpoint_list);

			if constexpr(!is_string<decltype(host)>::value || !is_string<decltype(port)>::value)
				throw std::overflow_error("Usage: client <host> <port> : type - string");

			boost::asio::ip::tcp::resolver resolver(io_service);

			auto endpoints = resolver.resolve(std::get<0>(endpoint_list), std::get<1>(endpoint_list));
			do_connect(endpoints);
		}

	public:
		template<class _Ty, std::size_t N>
		void async_write(const std::array<_Ty, N>& buf)
		{
			boost::asio::async_write(socket_, boost::asio::buffer(buf),
									 [this](boost::system::error_code ec, std::size_t)
									 {
										 if(ec)
										 {
											 std::cout << ec.message() << std::endl;
										 }
									 });
		}

		void async_write(ftstream&& buf)
		{
			boost::asio::async_write(socket_, boost::asio::buffer(buf.read_pointer(), buf.active()),
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
										   if(ec)
											   return;

										   do_read();
									   });
		}

		void do_read()
		{
			boost::asio::async_read(socket_, boost::asio::buffer(buffer_),
									[this](boost::system::error_code ec, std::size_t)
									{
										if(ec)
											return;

										read_handler();

										do_read();
									});
		}

	private:
		boost::asio::io_service& io_service_;

		boost::asio::ip::tcp::socket socket_;

		std::array<std::byte, 8192> buffer_;
	};
}