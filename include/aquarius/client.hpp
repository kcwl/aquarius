#pragma once
#include <aquarius/context/invoke.hpp>
#include <aquarius/core/flex_buffer.hpp>
#include <aquarius/core/type_traits.hpp>
#include <aquarius/response.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <map>
#include <type_traits>

namespace aquarius
{
	class client : public std::enable_shared_from_this<client>
	{
	public:
		explicit client()
			: io_service_()
			, socket_(io_service_)
		{}

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
		void open(const std::string& host, const std::string& port)
		{
			boost::asio::ip::tcp::resolver resolver(io_service_);

			auto endpoints = resolver.resolve(host, port);
			do_connect(endpoints);

			run();
		}

		void run()
		{
			thread_ptr_.reset(new std::thread([&] { io_service_.run(); }));
		}

		void shut_down()
		{
			io_service_.stop();

			thread_ptr_->join();

			boost::system::error_code ec;
			socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);

			socket_.close();
		}

		template <class _Ty, std::size_t N>
		void async_write(const std::array<_Ty, N>& buf)
		{
			// auto self = this->shared_from_this();

			boost::asio::async_write(socket_, boost::asio::buffer(buf),
									 [this /*, self*/](const boost::system::error_code& ec, std::size_t)
									 {
										 if (ec)
										 {
											 std::cout << ec.message() << std::endl;
										 }
									 });
		}

		void async_write(core::flex_buffer_t&& buf)
		{
			// auto self = this->shared_from_this();

			boost::asio::async_write(socket_, boost::asio::buffer(buf.rdata(), buf.size()),
									 [this /*, self*/](boost::system::error_code ec, std::size_t)
									 {
										 if (ec)
										 {
											 std::cout << ec.message() << std::endl;
										 }
									 });
		}

		template <typename _Message>
		void async_write(_Message&& msg)
		{
			aquarius::core::flex_buffer_t buffer{};
			msg.visit(buffer, core::visit_mode::output);

			async_write(std::move(buffer));
		}

		template <typename _Message, typename _Func>
		void async_write(_Message&& msg, _Func&& f)
		{
			async_funcs_.emplace(msg.header().magic_, std::forward<_Func>(f));

			async_write(std::forward<_Message>(msg));
		}

		virtual int read_handler() = 0;

		void close()
		{
			socket_.close();
		}

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
			// auto self = this->shared_from_this();

			boost::asio::async_connect(socket_, endpoints,
									   [this /*, self*/](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
									   {
										   if (ec)
											   return;

										   do_read();
									   });
		}

		void do_read()
		{
			// auto self = this->shared_from_this();

			buffer_.ensure();
			buffer_.normalize();
			socket_.async_read_some(boost::asio::buffer(buffer_.wdata(), buffer_.active()),
									[this /*, self*/](boost::system::error_code ec, std::size_t bytes_transferred)
									{
										if (ec)
											return;

										buffer_.commit(static_cast<int>(bytes_transferred));

										int pos = static_cast<int>(buffer_.size());

										uint32_t proto = 0;

										elastic::binary_iarchive ia(buffer_);

										ia >> proto;

										pos -= buffer_.size();

										auto req = ctx::message_invoke_helpter::invoke(proto);

										if (!req)
											return;

										req->visit(buffer_, aquarius::core::visit_mode::input);

										auto iter = async_funcs_.find(req->unique_key());

										if (iter == async_funcs_.end())
										{
											buffer_.consume(-req->size() - pos);

											read_handler();
										}
										else
										{
											iter->second(req);

											async_funcs_.erase(iter);
										}

										do_read();
									});
		}

	private:
		boost::asio::io_service io_service_;

		boost::asio::ip::tcp::socket socket_;

		core::flex_buffer_t buffer_;

		std::shared_ptr<std::thread> thread_ptr_;

		std::map<uint32_t, std::function<void(std::shared_ptr<tcp::xmessage>)>> async_funcs_;
	};
} // namespace aquarius