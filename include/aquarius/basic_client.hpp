#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/client_context.hpp>
#include <aquarius/co_spawn.hpp>
#include <aquarius/deadline_timer.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_context.hpp>
#include <aquarius/ip/address.hpp>
#include <aquarius/ip/basic_endpoint.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/redirect_error.hpp>
#include <aquarius/use_awaitable.hpp>
#include <aquarius/use_future.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>

namespace aquarius
{
	template <typename Session>
	class basic_client : public std::enable_shared_from_this<basic_client<Session>>
	{
		using session_type = Session;

		using socket_type = typename session_type::socket_type;

		using protocol_type = typename session_type::protocol_type;

		using endpoint_type = ip::basic_endpoint<protocol_type>;

	public:
		basic_client(int reconnect_times = 3, std::chrono::steady_clock::duration timeout = 30ms)
			: io_service_()
			, session_ptr_(nullptr)
			, timer_(io_service_)
			, reconnect_times_(reconnect_times)
			, timeout_(timeout)
			, ip_addr_()
			, port_()
			, thread_ptr_(nullptr)
		{}

		explicit basic_client(const std::string& ip_addr, const std::string& port, int reconnect_times = 3,
							  std::chrono::steady_clock::duration timeout = 30ms)
			: basic_client(reconnect_times, timeout)
		{
			async_connect(ip_addr, port);
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

			if (thread_ptr_ && thread_ptr_->joinable())
			{
				thread_ptr_->join();
			}
		}

		void close()
		{
			session_ptr_->close();
		}

		bool async_connect(const std::string& ip_addr, const std::string& port)
		{
			ip_addr_ = ip_addr;
			port_ = port;

			socket_type socket(io_service_);

			auto future = co_spawn(
				io_service_,
				[reconnect_times = this->reconnect_times_, ip_addr, port, sock = std::move(socket),
				 this] mutable -> awaitable<bool>
				{
					error_code ec{};

					auto addr = ip::make_address(ip_addr.c_str(), ec);

					if (ec)
					{
						co_return false;
					}

					endpoint_type endpoint(addr, static_cast<ip::port_type>(std::atoi(port.c_str())));

					co_await sock.async_connect(endpoint, redirect_error(use_awaitable, ec));

					if (!create_session(std::move(sock)))
					{
						co_return false;
					}

					boost::asio::co_spawn(this->io_service_, [reconnect_times, this] mutable->boost::asio::awaitable<void>
						{
							auto ec = co_await session_ptr_->async_read();

							if (ec)
							{
								while (reconnect_times--)
								{
									if (async_connect(ip_addr_, port_))
										break;
								}

								if (reconnect_times == 0)
								{
									co_return;
								}
							}
						}, boost::asio::detached);

					co_return true;
				},
				use_future);

			thread_ptr_ = std::make_shared<std::thread>([&] { this->io_service_.run(); });

			return future.get();
		}

		template <typename RPC, typename Request = typename RPC::tcp_request, typename Response = RPC::tcp_response>
		std::optional<Response> async_send(Request req)
		{
			flex_buffer fs{};
			req.to_binary(fs);

			std::promise<Response> resp_promise{};
			auto future = resp_promise.get_future();

			client_invoke::regist<Response>(random_session_id(),
											[promise = std::move(resp_promise)](Response resp) mutable
											{ promise.set_value(std::move(resp)); });

			co_spawn(io_service_, session_ptr_->async_send(Request::proto, std::move(fs)), detached);

			return future.get();
		}

		template <typename RPC, typename Func, typename Request = typename RPC::tcp_request,
				  typename Response = typename RPC::tcp_response>
		void async_send(Request req, Func&& f)
		{
			flex_buffer fs{};
			req.to_binary(fs);

			client_invoke::regist<Response>(random_session_id(), std::forward<Func>(f));

			co_spawn(io_service_, session_ptr_->async_send(Request::proto, std::move(fs)), detached);
		}

		void async_send(flex_buffer buffer)
		{
			co_spawn(
				io_service_,
				[buf = std::move(buffer), this] -> awaitable<void>
				{
					error_code ec{};
					co_await session_ptr_->async_send(std::move(buf), ec);
				},
				detached);
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

		template <typename Func>
		void async_wait(std::chrono::system_clock::duration dura, Func&& f)
		{
			error_code ec;
			timer_.expires_after(dura);

			timer_.async_wait(
				[&](const error_code& ec)
				{
					if (ec)
						return;

					if (timer_.expiry() <= std::chrono::steady_clock::now())
					{
						f();
						return;
					}

					async_wait(dura, std::forward<Func>(f));
				});
		}

		template <typename Func>
		void set_close_func(Func&& f)
		{
			close_func_ = std::forward<Func>(f);
		}

	private:
		std::size_t random_session_id()
		{
			return 0;
		}

		bool create_session(socket_type socket)
		{
			session_ptr_ = std::make_shared<session_type>(std::move(socket));

			session_ptr_->regist_close(close_func_);

			return true;
		}

	private:
		io_context io_service_;

		std::shared_ptr<Session> session_ptr_;

		steady_timer timer_;

		std::function<void()> close_func_;

		int reconnect_times_;

		std::chrono::steady_clock::duration timeout_;

		std::string ip_addr_;

		std::string port_;

		std::shared_ptr<std::thread> thread_ptr_;
	};
} // namespace aquarius