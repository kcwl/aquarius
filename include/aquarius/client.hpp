#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/response.hpp>
#include <aquarius/type_traits.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <map>
#include <type_traits>
#include <filesystem>

namespace aquarius
{
	template <typename _Connector>
	class client : public std::enable_shared_from_this<client<_Connector>>
	{
	public:
		template <class... _Args, class = std::enable_if_t<(sizeof...(_Args) > 1)>>
		explicit client(_Args&&... args)
			: io_service_()
			, ssl_context_(boost::asio::ssl::context::sslv23)
			, resolve_(io_service_)
		{
			init_ssl_context();

			if constexpr (sizeof...(args) < 2)
				std::throw_with_nested(std::overflow_error("Usage: client <host> <port>"));

			auto endpoint_list = std::make_tuple(std::forward<_Args>(args)...);

			auto host = std::get<0>(endpoint_list);
			auto port = std::get<1>(endpoint_list);

			static_assert(detail::is_string_v<decltype(host)> && detail::is_string_v<decltype(port)>, "Usage: client <host> <port> : type - string");

			do_connect(resolve_.resolve(host, port));
		}

	public:
		void run()
		{
			io_service_.run();
		}

		void stop()
		{
			io_service_.stop();
		}

		template <typename _Request, typename _Func>
		void async_write(_Request&& req, _Func&& f)
		{
			if (!conn_ptr_)
				return;

			flex_buffer_t fs{};

			req.to_binary(fs);

			conn_ptr_->async_write(std::move(fs), std::forward<_Func>(f));
		}

		template <typename _Request>
		void async_write(_Request&& req)
		{
			async_write(std::forward<_Request>(req), [] {});
		}

		template <typename _Request, typename _Func>
		void async_write(std::size_t uid, _Request&& req, _Func&& f)
		{
			if (!conn_ptr_)
				return;

			flex_buffer_t fs{};

			req.to_binary(fs);

			conn_ptr_->async_write(uid, std::move(fs), std::forward<_Func>(f));
		}

		template <typename _Func>
		void async_write(std::size_t uid, flex_buffer_t&& buffer, _Func&& f)
		{
			if (!conn_ptr_)
				return;

			conn_ptr_->async_write(uid, std::move(buffer), std::forward<_Func>(f));
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
			conn_ptr_ = std::make_shared<_Connector>(io_service_, ssl_context_);

			if (!conn_ptr_)
				return;

			boost::asio::async_connect(conn_ptr_->socket(), endpoints,
									   [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint)
									   {
										   if (ec)
											   return;

										   if (!conn_ptr_)
											   return;

										   conn_ptr_->set_verify_mode(boost::asio::ssl::verify_peer);

										   conn_ptr_->start();
									   });
		}

		void init_ssl_context()
		{
			auto path = std::filesystem::current_path();

			path.append("crt");
			path.append("server.crt");

			ssl_context_.load_verify_file(path.string());
		}

	private:
		boost::asio::io_service io_service_;

		boost::asio::ssl::context ssl_context_;

		std::shared_ptr<_Connector> conn_ptr_;

		boost::asio::ip::tcp::resolver::results_type endpoint_;

		boost::asio::ip::tcp::resolver resolve_;
	};
} // namespace aquarius