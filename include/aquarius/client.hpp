#pragma once
#include <aquarius/core/type_traits.hpp>
#include <aquarius/core/flex_buffer.hpp>
#include <aquarius/response.hpp>
#include <boost/asio.hpp>
#include <type_traits>
#include <iostream>

namespace aquarius
{
	class client
	{
	public:
		explicit client()
			: io_service_()
			, socket_(io_service_)
		{

		}

		client(/*boost::asio::io_service& io_service,*/
						const boost::asio::ip::tcp::resolver::results_type& endpoints)
			: io_service_(/*io_service*/)
			, socket_(io_service_)
			, buffer_()
		{
			do_connect(endpoints);
		}

		template <class... _Args, class = std::enable_if_t<(sizeof...(_Args) > 1)>>
		client(/*boost::asio::io_service& io_service,*/ _Args&&... args)
			: io_service_(/*io_service*/)
			, socket_(io_service_)
		{
			if constexpr (sizeof...(args) < 2)
				std::throw_with_nested(std::overflow_error("Usage: client <host> <port>"));

			auto endpoint_list = std::make_tuple(std::forward<_Args>(args)...);

			auto host = std::get<0>(endpoint_list);
			auto port = std::get<1>(endpoint_list);

			if constexpr (!core::is_string<decltype(host)>::value || !core::is_string<decltype(port)>::value)
				throw std::overflow_error("Usage: client <host> <port> : type - string");

			boost::asio::ip::tcp::resolver resolver(io_service_);

			auto endpoints = resolver.resolve(std::get<0>(endpoint_list), std::get<1>(endpoint_list));
			do_connect(endpoints);
		}

	public:
		void open(boost::asio::io_service& io_service, const std::string& host, const std::string& port)
		{
			boost::asio::ip::tcp::resolver resolver(io_service);

			auto endpoints = resolver.resolve(host, port);
			do_connect(endpoints);

			run();
		}

		void run()
		{
			thread_ptr_.reset(new std::thread([&] { io_service_.run(); }));
		}

		template <class _Ty, std::size_t N>
		void async_write(const std::array<_Ty, N>& buf)
		{
			boost::asio::async_write(socket_, boost::asio::buffer(buf),
									 [this](const boost::system::error_code& ec, std::size_t)
									 {
										 if (ec)
										 {
											 std::cout << ec.message() << std::endl;
										 }
									 });
		}

		void async_write(core::flex_buffer_t&& buf)
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

		void async_write(const void* data, std::size_t size)
		{
			aquarius::core::flex_buffer_t buffer(data, size);

			async_write(std::move(buffer));
		}

		virtual int read_handler() = 0;

		void close() { socket_.close(); }

		core::flex_buffer_t& get_recv_buffer()
		{
			return buffer_;
		}

		std::string remote_address()
		{
			return socket_.remote_endpoint().address().to_string();
		}

		int remote_port()
		{
			return socket_.remote_endpoint().port();
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
			buffer_.ensure();
			buffer_.normalize();
			socket_.async_read_some(boost::asio::buffer(buffer_.wdata(), buffer_.active()),
									[this](boost::system::error_code ec, std::size_t bytes_transferred)
									{
										if (ec)
											return;

										buffer_.commit(static_cast<int>(bytes_transferred));

										//if (!system_call(bytes_transferred))
											read_handler();

										do_read();
									});
		}

		bool system_call(std::size_t bytes_transferred)
		{
			elastic::binary_iarchive ia(buffer_);

			int proto_id = 0;

			ia >> proto_id;

			if (proto_id != 1000)
			{
				return false;
			}

			buffer_.consume(static_cast<int>(bytes_transferred));

			null_body_response<1001> resp{};

			core::flex_buffer_t fs;

			resp.visit(fs, aquarius::core::visit_mode::output);

			//async_write(std::move(fs));

			std::cout << "pong\n";
			return true;
		}

	private:
		boost::asio::io_service io_service_;

		boost::asio::ip::tcp::socket socket_;

		core::flex_buffer_t buffer_;

		std::shared_ptr<std::thread> thread_ptr_;
	};
} // namespace aquarius