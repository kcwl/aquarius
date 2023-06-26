#pragma once
#include <aquarius/detail/invoke.hpp>
#include <aquarius/detail/type_traits.hpp>
#include <aquarius/flex_buffer.hpp>
#include <aquarius/response.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <map>
#include <type_traits>

namespace aquarius
{
	template <typename _Connector>
	class client : public std::enable_shared_from_this<client<_Connector>>
	{
	public:
		explicit client()
			: io_service_()
		{}

		client(const boost::asio::ip::tcp::resolver::results_type& endpoints)
			: client()
		{
			do_connect(endpoints);
		}

		template <class... _Args, class = std::enable_if_t<(sizeof...(_Args) > 1)>>
		client(_Args&&... args)
			: client()
		{
			if constexpr (sizeof...(args) < 2)
				std::throw_with_nested(std::overflow_error("Usage: client <host> <port>"));

			auto endpoint_list = std::make_tuple(std::forward<_Args>(args)...);

			auto host = std::get<0>(endpoint_list);
			auto port = std::get<1>(endpoint_list);

			if constexpr (!detail::is_string_v<decltype(host)> || !detail::is_string_v<decltype(port)>)
				throw std::overflow_error("Usage: client <host> <port> : type - string");

			boost::asio::ip::tcp::resolver resolver(io_service_);

			auto endpoints = resolver.resolve(std::get<0>(endpoint_list), std::get<1>(endpoint_list));
			do_connect(endpoints);
		}

	public:
		void run()
		{
			io_service_.run();
		}

		void shut_down()
		{
			io_service_.stop();

			conn_ptr_->shut_down();
		}

		void async_write(flex_buffer_t&& buf)
		{
			conn_ptr_->async_write(std::forward<flex_buffer_t>(buf));
		}

		std::string remote_address()
		{
			return conn_ptr_->remote_address();
		}

		int remote_port()
		{
			return conn_ptr_->remote_port();
		}

	private:
		void do_connect(boost::asio::ip::tcp::resolver::results_type endpoints)
		{
			conn_ptr_ = std::make_shared<_Connector>(io_service_);

			boost::asio::async_connect(conn_ptr_->socket(), endpoints,
									   [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
									   {
										   if (ec)
											   return;

										   conn_ptr_->start();
									   });
		}

	private:
		boost::asio::io_service io_service_;

		std::shared_ptr<_Connector> conn_ptr_;
	};
} // namespace aquarius