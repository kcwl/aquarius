#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/message/invoke.hpp>
#include <aquarius/type_traits.hpp>
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
			, is_report_(false)
			, server_port_(0)
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

		template <typename _Request, typename _Func>
		void async_write(_Request&& req, _Func&& f)
		{
			flex_buffer_t fs{};

			req.to_binary(fs);

			conn_ptr_->async_write(std::move(fs), std::forward<_Func>(f));
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

		void set_report(bool report = true)
		{
			is_report_ = report;
		}

		void set_server_port(int32_t port)
		{
			server_port_ = port;
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

										   if (!conn_ptr_)
											   return;

										   if (is_report_)
										   {
											   flex_buffer_t buffer{};
											   buffer.sputn(ip_report_proto, sizeof(ip_report_proto));

											   request_header header{};
											   header.session_id_ = server_port_;

											   buffer.sputn((char*)&header, sizeof(header));

											   conn_ptr_->async_write(std::move(buffer), []{});
										   }

										   conn_ptr_->start();
									   });
		}

	private:
		boost::asio::io_service io_service_;

		std::shared_ptr<_Connector> conn_ptr_;

		boost::asio::ip::tcp::resolver::results_type endpoint_;

		bool is_report_;

		int32_t server_port_;
	};
} // namespace aquarius