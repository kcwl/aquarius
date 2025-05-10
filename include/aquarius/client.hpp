#pragma once
#include <aquarius/core/asio.hpp>
#include <aquarius/core/concepts.hpp>
#include <aquarius/core/logger.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>

namespace aquarius
{
	template <typename Protocol, typename Session>
	class client : public std::enable_shared_from_this<client<Protocol, Session>>
	{
		using socket_type = boost::asio::basic_stream_socket<Protocol>;

		using resolve_type = boost::asio::ip::basic_resolver<Protocol>;

	public:
		explicit client(const std::string& ip_addr, const std::string& port)
			: io_service_()
		{
			boost::asio::co_spawn(io_service_, do_connect(ip_addr, port), boost::asio::detached);
		}

	public:
		void run()
		{
			io_service_.run();
		}

		void stop()
		{
			io_service_.stop();

			session_ptr_->shutdown();
		}

		template <typename Request>
		void send_request(const Request& req)
		{
			flex_buffer_t fs{};

			req.template to_binary<flex_buffer_t>(fs);

			async_write(std::move(fs));
		}

		void async_write(flex_buffer_t&& buffer)
		{
			 session_ptr_->send_packet(std::move(buffer));
		}

		std::string remote_address()
		{
			return session_ptr_->remote_address();
		}

		uint32_t remote_address_u()
		{
			return session_ptr_->remote_address_u();
		}

		int remote_port()
		{
			return session_ptr_->remote_port();
		}

	private:
		auto do_connect(const std::string& ip_addr, const std::string& port) -> boost::asio::awaitable<void>
		{
			socket_type socket(io_service_);

			boost::system::error_code ec;

			resolve_type resolver(io_service_);
			auto endpoints = resolver.resolve(ip_addr, port);

			co_await boost::asio::async_connect(socket, endpoints, boost::asio::redirect_error(boost::asio::use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << " maybe occur error - " << ec.message();

				co_return;
			}

			session_ptr_ = std::make_shared<Session>(std::move(socket));

			session_ptr_->start();
		}

		// void init_ssl_context()
		//{
		//	auto path = std::filesystem::current_path();

		//	path.append("crt");
		//	path.append("server.crt");

		//	ssl_context_.load_verify_file(path.string());
		//}

	private:
		asio::io_service io_service_;

		std::shared_ptr<Session> session_ptr_;
	};
} // namespace aquarius