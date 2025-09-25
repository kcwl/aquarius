#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/redirect_error.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/io_context.hpp>
#include <aquarius/logger.hpp>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <boost/asio/connect.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/coroutine.hpp>

namespace aquarius
{
	template <typename Session, typename... Adaptor>
	class basic_client
	{
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
		{
			
		}

		virtual ~basic_client() =default;

	public:
		auto async_connect(const std::string& ip_addr, const std::string& port) -> awaitable<bool>
		{
			host_ = ip_addr;
			port_ = port;

			session_ptr_ = std::make_shared<Session>(io_context_);

			auto result = co_await session_ptr_->async_connect(host_, port_);

			if (!result)
				co_return result;

			co_spawn(
				this->io_context_,
				[this]() mutable -> awaitable<void>
				{
					auto ec = co_await session_ptr_->accept();
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
					co_return;
				},
				use_awaitable);

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

		template <typename Request>
		auto async_send(std::shared_ptr<Request> req) -> awaitable<void>
		{
			//detail::flex_buffer<char> complete_buffer;
			//req->mark(complete_buffer);
			//auto header_size = complete_buffer.active();

			//auto result = req->commit(complete_buffer);

			//if (!result.has_value())
			//{
			//	co_return;
			//}

			//req->length(complete_buffer.active() - header_size);
			//result = req->header().commit(complete_buffer);

			//if (!result.has_value())
			//	co_return;

			//co_return co_await transfer(std::move(complete_buffer));
			co_return;
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

		void close()
		{
			return session_ptr_->close();
		}

		void shutdown()
		{
			return session_ptr_->shutdown();
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
		io_context& io_context_;

		std::shared_ptr<Session> session_ptr_;

		std::function<void()> close_func_;

		int reconnect_times_;

		std::chrono::steady_clock::duration timeout_;

		std::string host_;

		std::string port_;

		std::tuple<Adaptor...> adaptors_;
	};

	template <typename Session, typename... Adaptor>
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

		std::tuple<Adaptor...> adaptors_;
	};
} // namespace aquarius