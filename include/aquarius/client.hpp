#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/message/invoke.hpp>
#include <aquarius/response.hpp>
#include <aquarius/type_traits.hpp>
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
		template <class... _Args, class = std::enable_if_t<(sizeof...(_Args) > 1)>>
		explicit client(boost::asio::io_service& ios, _Args&&... args)
			: io_service_(ios)
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
											   buffer.sputn((uint8_t*)&ip_report_proto, sizeof(ip_report_proto));

											   request_header header{};
											   header.session_id_ = server_port_;

											   buffer.sputn((uint8_t*)&header, sizeof(header));

											   conn_ptr_->async_write(std::move(buffer), [] {});
										   }

										   conn_ptr_->start();
									   });
		}

	private:
		boost::asio::io_service& io_service_;

		std::shared_ptr<_Connector> conn_ptr_;

		boost::asio::ip::tcp::resolver::results_type endpoint_;

		bool is_report_;

		int32_t server_port_;
	};

	template <typename _Connector>
	class alone_client
	{
	public:
		explicit alone_client(const std::string& ip_addr, int32_t port)
			: io_service_()
			, client_ptr_(new client<_Connector>(io_service_, ip_addr, std::to_string(port)))
		{}

	public:
		void run()
		{
			io_service_.run();
		}

		void stop()
		{
			io_service_.stop();
		}

		template<typename _Ty, typename _Func>
		void async_write(_Ty&& request, _Func&& f)
		{
			if (!client_ptr_)
				return;

			client_ptr_->async_write(std::forward<_Ty>(request), std::forward<_Func>(f));
		}

	private:
		boost::asio::io_service io_service_;

		std::shared_ptr<client<_Connector>> client_ptr_;
	};

} // namespace aquarius