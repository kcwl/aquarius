﻿#pragma once
#include <aquarius/awaitable.hpp>
#include <aquarius/co_spawn.hpp>
#include <aquarius/detached.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/redirect_error.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_context.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/traits/async.hpp>
#include <aquarius/use_awaitable.hpp>
#include <aquarius/use_future.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>

namespace aquarius
{
	template <typename Session>
	class basic_client
	{
		using session = Session;

		using socket = typename Session::socket;

		using resolver = typename Session::resolver;

	public:
		basic_client(io_context& context, int reconnect_times, std::chrono::steady_clock::duration timeout)
			: io_context_(context)
			, session_ptr_(nullptr)
			, reconnect_times_(reconnect_times)
			, timeout_(timeout)
			, host_()
			, port_()
		{}

		virtual ~basic_client()
		{
			stop();
		}

	public:
		auto async_connect(const std::string& ip_addr, const std::string& port) -> awaitable<bool>
		{
			host_ = ip_addr;
			port_ = port;

			socket _socket(io_context_);

			resolver resolve(io_context_);

			auto endpoints = resolve.resolve(host_, port_);

			error_code ec{};

			co_await boost::asio::async_connect(_socket, endpoints, redirect_error(use_awaitable, ec));

			if (ec)
			{
				XLOG_ERROR() << "connect " << host_ << " failed! maybe" << ec.what();
				co_return false;
			}

			create_session(std::move(_socket));

			co_spawn(
				this->io_context_,
				[this]() mutable -> awaitable<void>
				{
					auto ec = co_await session_ptr_->protocol();
					if (ec)
					{
						if (close_func_)
							this->close_func_();

						auto reconnect_times = this->reconnect_times_;

						while (reconnect_times--)
						{
							if (co_await reconnect())
								break;
						}
					}
				},
				detached);

			co_return true;
		}

		auto reconnect() -> awaitable<bool>
		{
			if (host_.empty() || port_.empty())
				co_return false;

			if (io_context_.stopped()) [[likely]]
			{
				io_context_.restart();
			}

			co_return co_await async_connect(host_, port_);
		}

		template <typename RPC>
		auto async_send(std::shared_ptr<typename RPC::request> req) -> awaitable<void>
		{
			std::vector<char> fs{};
			req->commit(fs);

			std::vector<char> complete_buffer;
			req->mark(complete_buffer);
			auto header_size = complete_buffer.size();
			req->rpc(RPC::id);
			req->commit(complete_buffer);
			req->length(complete_buffer.size() - header_size);
			req->header().commit(complete_buffer);

			co_return co_await transfer(std::move(complete_buffer));
		}

		template <typename BuffSequence>
		auto transfer(BuffSequence buffer) -> awaitable<void>
		{
			co_await session_ptr_->async_send(std::move(buffer));
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

		virtual void stop()
		{
			if (session_ptr_)
			{
				session_ptr_->shutdown();
			}
		}

		auto& get_context()
		{
			return io_context_;
		}

	private:
		void create_session(socket sock)
		{
			session_ptr_ = std::make_shared<session>(std::move(sock));
		}

	private:
		io_context& io_context_;

		std::shared_ptr<session> session_ptr_;

		std::function<void()> close_func_;

		int reconnect_times_;

		std::chrono::steady_clock::duration timeout_;

		std::string host_;

		std::string port_;
	};

	template <typename Session>
	class basic_exector_client : public basic_client<Session>
	{
		using base = basic_client<Session>;

	public:
		basic_exector_client(int reconnect_times, std::chrono::steady_clock::duration timeout)
			: base(io_context_, reconnect_times, timeout)
			, thread_ptr_(nullptr)
		{}

		virtual ~basic_exector_client() = default;

	public:
		void async_run()
		{
			if (!thread_ptr_ || !thread_ptr_->joinable())
			{
				thread_ptr_ = std::make_shared<std::thread>([&] { this->io_context_.run(); });
			}
		}

		virtual void stop()
		{
			base::stop();

			if (!io_context_.stopped())
			{
				io_context_.stop();
			}

			if (thread_ptr_ && thread_ptr_->joinable())
			{
				thread_ptr_->join();
			}
		}

	private:
		io_context io_context_;

		std::shared_ptr<std::thread> thread_ptr_;
	};

	template <typename Session>
	struct is_client<basic_client<Session>> : std::true_type
	{};

	template <typename Session>
	struct is_client<basic_exector_client<Session>> : std::true_type
	{};

} // namespace aquarius