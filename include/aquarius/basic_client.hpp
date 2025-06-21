#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/co_spawn.hpp>
#include <aquarius/context/client_router.hpp>
#include <aquarius/context/context.hpp>
#include <aquarius/deadline_timer.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_context.hpp>
#include <aquarius/ip/address.hpp>
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
	template <typename Protocol>
	class basic_client
	{
		using session = Protocol::client_session;
		using socket = typename Protocol::socket;
		using resolver = typename Protocol::resolver;
		using port_type = uint16_t;

	public:
		basic_client(int reconnect_times = 3, std::chrono::steady_clock::duration timeout = 100000ms)
			: io_context_()
			, session_ptr_(nullptr)
			, reconnect_times_(reconnect_times)
			, timeout_(timeout)
			, ip_addr_()
			, port_()
			, thread_ptr_(nullptr)
		{}

		virtual ~basic_client()
		{
			stop();
		}

	public:
		bool async_connect(const std::string& ip_addr, const std::string& port)
		{
			ip_addr_ = ip_addr;
			port_ = port;

			auto future = co_spawn(
				io_context_,
				[this] mutable -> awaitable<error_code>
				{
					socket _socket(io_context_);

					resolver resolve(io_context_);

					auto endpoints = resolve.resolve(ip_addr_, port_);

					error_code ec{};

					co_await boost::asio::async_connect(_socket, endpoints, redirect_error(use_awaitable, ec));

					if (ec)
					{
						XLOG_ERROR() << "connect " << ip_addr_ << " failed! maybe" << ec.what();
					}
					else
					{
						create_session(std::move(_socket));

						co_spawn(
							this->io_context_,
							[this] mutable -> awaitable<void>
							{
								auto ec = co_await session_ptr_->protocol();
								if (ec)
								{
									auto reconnect_times = this->reconnect_times_;

									while (reconnect_times--)
									{
										if (async_connect(ip_addr_, port_))
											break;
									}
								}
							},
							detached);
					}

					co_return ec;
				},
				use_future);

			if (!thread_ptr_)
			{
				thread_ptr_ = std::make_shared<std::thread>([&] { this->io_context_.run(); });
			}

			return !future.get();
		}
		template <typename RPC>
		std::optional<typename RPC::response> async_send(typename RPC::request req)
		{
			std::vector<char> fs{};
			req.pack(fs);
			auto promise_ptr = std::make_shared<std::promise<std::optional<typename RPC::response>>>();
			auto future = promise_ptr->get_future();

			int id = 0;
			context::detail::client_router::get_mutable_instance().regist<typename RPC::response>(
				id, [promise_ptr](std::optional<typename RPC::response> resp) mutable
				{ promise_ptr->set_value(std::move(resp)); });

			error_code ec{};

			async_send(std::move(fs), static_cast<uint8_t>(mode::stream), RPC::id, ec);

			auto status = future.wait_for(timeout_);

			return status == std::future_status::timeout ? std::nullopt : future.get();
		}

		void async_send(std::vector<char> buffer, uint8_t mode, std::size_t rpc_id, error_code& ec)
		{
			co_spawn(
				io_context_, [buf = std::move(buffer), mode, rpc_id, this, &ec] -> awaitable<void>
				{ co_await session_ptr_->async_send(std::move(buf), mode, rpc_id, ec); }, detached);
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
		void set_close_func(Func&& f)
		{
			close_func_ = std::forward<Func>(f);
		}

		auto get_executor()
		{
			return io_context_.get_executor();
		}

		void stop()
		{
			if (!io_context_.stopped())
			{
				io_context_.stop();
			}

			if (session_ptr_)
			{
				session_ptr_->shutdown();
			}

			if (thread_ptr_ && thread_ptr_->joinable())
			{
				thread_ptr_->join();
			}
		}

	private:
		void create_session(socket socket)
		{
			session_ptr_ = std::make_shared<session>(std::move(socket));
			session_ptr_->regist_close(close_func_);
		}

	private:
		io_context io_context_;
		std::shared_ptr<session> session_ptr_;
		std::function<void()> close_func_;
		int reconnect_times_;
		std::chrono::steady_clock::duration timeout_;
		std::string ip_addr_;
		std::string port_;
		std::shared_ptr<std::thread> thread_ptr_;
	};
} // namespace aquarius