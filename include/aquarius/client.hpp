#pragma once
#include <aquarius/detail/type_traits.hpp>
#include <aquarius/impl/io.hpp>
#include <boost/asio.hpp>
#include <type_traits>
#include <aquarius/impl/header.hpp>

namespace aquarius
{
	class client
	{
	public:
		explicit client(boost::asio::io_service& io_service,
						const boost::asio::ip::tcp::resolver::results_type& endpoints)
			: io_service_(io_service)
			, socket_(io_service)
			, buffer_()
		{
			do_connect(endpoints);
		}

		template <class... _Args, class = std::enable_if_t<(sizeof...(_Args) > 1)>>
		client(boost::asio::io_service& io_service, _Args&&... args)
			: io_service_(io_service)
			, socket_(io_service)
		{
			if constexpr (sizeof...(args) < 2)
				std::throw_with_nested(std::overflow_error("Usage: client <host> <port>"));

			auto endpoint_list = std::make_tuple(std::forward<_Args>(args)...);

			auto host = std::get<0>(endpoint_list);
			auto port = std::get<1>(endpoint_list);

			if constexpr (!detail::is_string<decltype(host)>::value || !detail::is_string<decltype(port)>::value)
				throw std::overflow_error("Usage: client <host> <port> : type - string");

			boost::asio::ip::tcp::resolver resolver(io_service);

			auto endpoints = resolver.resolve(std::get<0>(endpoint_list), std::get<1>(endpoint_list));
			do_connect(endpoints);
		}

	public:
		template <class _Ty, std::size_t N>
		void async_write(const std::array<_Ty, N>& buf)
		{
			boost::asio::async_write(socket_, boost::asio::buffer(buf),
									 [this](boost::system::error_code ec, std::size_t)
									 {
										 if (ec)
										 {
											 std::cout << ec.message() << std::endl;
										 }
									 });
		}

		void async_write(impl::flex_buffer_t&& buf)
		{
			boost::asio::async_write(socket_, boost::asio::buffer(buf.rdata(), buf.size()),
									 [this](boost::system::error_code ec, std::size_t)
									 {
										 if (ec)
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
			socket_.async_read_some(boost::asio::buffer(buffer_.wdata(), buffer_.active()),
									[this](boost::system::error_code ec, std::size_t bytes_transferred)
									{
										if (ec)
											return;

										buffer_.commit(static_cast<int>(bytes_transferred));

										if (!system_call())
											read_handler();

										do_read();
									});
		}

		bool system_call()
		{
			elastic::binary_iarchive ia(buffer_);

			int proto_id = 0;

			ia >> proto_id;

			if (proto_id != 1000)
			{
				return false;
			}

			buffer_.consume(sizeof(aquarius::impl::tcp_request_header) + 1);

			// core::ping_response pr{};

			impl::flex_buffer_t fs;

			// pr.to_message(fs);

			async_write(std::move(fs));

			return true;
		}

	private:
		boost::asio::io_service& io_service_;

		boost::asio::ip::tcp::socket socket_;

		impl::flex_buffer_t buffer_;
	};
} // namespace aquarius