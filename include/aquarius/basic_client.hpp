#pragma once
#include <aquarius/logger.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <aquarius/detail/asio.hpp>

namespace aquarius
{
	template <typename Protocol, typename Session>
	class basic_client : public std::enable_shared_from_this<basic_client<Protocol, Session>>
	{
		using socket_type = boost::asio::basic_stream_socket<Protocol>;

		using endpoint_type = boost::asio::ip::basic_endpoint<Protocol>;

		using resolve_type = boost::asio::ip::basic_resolver<Protocol>;

	public:
		explicit basic_client(const std::string& ip_addr, const std::string& port)
			: io_service_()
		{
			do_connect(ip_addr, port);
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
		void send_request(Request& req)
		{
			flex_buffer_t fs = req.template to_binary<flex_buffer_t>();

			boost::asio::co_spawn(io_service_, session_ptr_->send_packet(Request::Number, fs), boost::asio::detached);
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

	public:
		void do_connect(const std::string& ip_addr, const std::string& port)
		{
			session_ptr_ = std::make_shared<Session>(io_service_);

			boost::asio::co_spawn(io_service_, session_ptr_->async_connect(ip_addr, port), boost::asio::detached);
		}

	private:
		boost::asio::io_service io_service_;

		std::shared_ptr<Session> session_ptr_;
	};
} // namespace aquarius