﻿#pragma once
#include <aquarius/client/callback.hpp>
#include <aquarius/context/invoke.hpp>
#include <aquarius/core/asio.hpp>
#include <aquarius/core/concepts.hpp>
#include <aquarius/core/logger.hpp>
#include <aquarius/response.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>

namespace aquarius
{
	template <typename _Connector>
	class client : public std::enable_shared_from_this<client<_Connector>>
	{
	public:
		explicit client(const std::string& ip_addr, const std::string& port,
						const std::function<void(bool)>& f = nullptr)
			: io_service_()
			, ssl_context_(asio::ssl::context::sslv23)
			, conn_ptr_(nullptr)
		{
			init_ssl_context();

			asio::ip::tcp::resolver resolve_(io_service_);

			do_connect(resolve_.resolve(ip_addr, port), f);
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

		void close(bool shutdown = false)
		{
			conn_ptr_->close(shutdown);
		}

		template <typename _Request, typename _Func>
		void send_request(_Request&& req, _Func&& f)
		{
			invoke_callback_helper::regist(req.uuid(), std::forward<_Func>(f));

			this->send_request(std::forward<_Request>(req));
		}

		template <typename _Request>
		void send_request(_Request&& req)
		{
			flex_buffer_t fs{};

			aquarius::error_code ec{};

			req.to_binary(fs, ec);

			async_write(std::move(fs));
		}

		void async_write(flex_buffer_t&& buffer)
		{
			auto session_ptr = invoke_session_helper::find(conn_ptr_->uuid());

			session_ptr->send_packet(std::move(buffer));
		}

		std::string remote_address()
		{
			return conn_ptr_->remote_address();
		}

		uint32_t remote_address_u()
		{
			return conn_ptr_->remote_address_u();
		}

		int remote_port()
		{
			return conn_ptr_->remote_port();
		}

		template <typename _Func>
		void regist_accept(_Func&& f)
		{
			conn_ptr_->regist_accept(std::forward<_Func>(f));
		}

		template <typename _Func>
		void regist_close(_Func&& f)
		{
			conn_ptr_->regist_close(std::forward<_Func>(f));
		}

	private:
		template <typename _Func>
		void do_connect(asio::ip::tcp::resolver::results_type endpoints, _Func&& f)
		{
			conn_ptr_ = std::make_shared<_Connector>(io_service_, ssl_context_);

			asio::async_connect(conn_ptr_->socket(), endpoints,
								[this, func = std::move(f)](boost::system::error_code ec, asio::ip::tcp::endpoint)
								{
									if (ec)
									{
										XLOG_ERROR() << " maybe occur error - " << ec.message();

										if (func)
											func(false);

										return;
									}

									conn_ptr_->start();

									if (func)
										func(true);
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
		asio::io_service io_service_;

		asio::ssl::context ssl_context_;

		std::shared_ptr<_Connector> conn_ptr_;
	};
} // namespace aquarius