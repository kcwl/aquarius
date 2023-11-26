#pragma once
#include <aquarius/message/invoke.hpp>
#include <aquarius/type_traits.hpp>
#include <aquarius/elastic.hpp>
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
		explicit client(const boost::asio::ip::tcp::resolver::results_type& endpoints)
			: io_service_()
			, endpoint_(endpoints)
		{
			do_connect(endpoint_);
		}

		template <class... _Args, class = std::enable_if_t<(sizeof...(_Args) > 1)>>
		client(_Args&&... args)
		{
			if constexpr (sizeof...(args) < 2)
				std::throw_with_nested(std::overflow_error("Usage: client <host> <port>"));

			auto endpoint_list = std::make_tuple(std::forward<_Args>(args)...);

			auto host = std::get<0>(endpoint_list);
			auto port = std::get<1>(endpoint_list);

			if constexpr (!detail::is_string_v<decltype(host)> || !detail::is_string_v<decltype(port)>)
				throw std::overflow_error("Usage: client <host> <port> : type - string");

			boost::asio::ip::tcp::resolver resolve(io_service_);

			do_connect(resolve.resolve(host, port));
		}

	public:
		void run()
		{
			io_service_.run();
		}

		void stop()
		{
			io_service_.stop();

			conn_ptr_->shut_down();
		}

		template <typename _Request>
		void async_write(_Request&& req)
		{
			flex_buffer_t fs{};
			aquarius::to_binary(_Request::Number, fs);

			aquarius::to_binary(req.size(), fs);

			req.to_binary(fs);

			conn_ptr_->async_write(std::move(fs));
		}

		std::string remote_address()
		{
			return conn_ptr_->remote_address();
		}

		int remote_port()
		{
			return conn_ptr_->remote_port();
		}

		void reconnect()
		{
			conn_ptr_->shut_down();

			do_connect(endpoint_);
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

		boost::asio::ip::tcp::resolver::results_type endpoint_;
	};
} // namespace aquarius