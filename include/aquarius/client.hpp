#pragma once
#include <aquarius/client/callback.hpp>
#include <aquarius/core/asio.hpp>
#include <aquarius/core/concepts.hpp>
#include <aquarius/core/logger.hpp>
#include <aquarius/response.hpp>
#include <filesystem>
#include <iostream>
#include <map>

namespace aquarius
{
	template <typename _Connector>
	class client : public std::enable_shared_from_this<client<_Connector>>
	{
	public:
		explicit client(const std::string& ip_addr, const std::string& port)
			: io_service_()
			, ssl_context_(asio::ssl::context::sslv23)
			, conn_ptr_(nullptr)
		{
			init_ssl_context();

			asio::ip::tcp::resolver resolve_(io_service_);

			do_connect(resolve_.resolve(ip_addr, port));
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
		void async_write(_Request&& req, _Func f)
		{
			invoke_callback_helper::regist(req.uuid(), std::forward<_Func>(f));

			this->async_write(std::forward<_Request>(req));
		}

		template <typename _Request>
		void async_write(_Request&& req)
		{
			flex_buffer_t fs{};

			req.to_binary(fs);

			async_write(std::move(fs));
		}

		void async_write(flex_buffer_t&& buffer)
		{
			conn_ptr_->async_write(std::move(buffer));
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

	private:
		void do_connect(asio::ip::tcp::resolver::results_type endpoints)
		{
			conn_ptr_ = std::make_shared<_Connector>(io_service_, ssl_context_);

			asio::async_connect(conn_ptr_->socket(), endpoints,
								[this](boost::system::error_code ec, asio::ip::tcp::endpoint)
								{
									if (ec)
									{
										XLOG_ERROR() << " maybe occur error - " << ec.message();

										return;
									}

									conn_ptr_->set_verify_mode(asio::ssl::verify_peer);

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
		asio::io_service io_service_;

		asio::ssl::context ssl_context_;

		std::shared_ptr<_Connector> conn_ptr_;
	};
} // namespace aquarius