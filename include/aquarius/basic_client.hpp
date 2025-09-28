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

	template <typename Session>
	class basic_client
	{
		using socket_t = Session::socket;

		using resolver = Session::resolver;

	public:
		basic_client(io_context& context, std::chrono::steady_clock::duration timeout)
			: io_context_(context)
			, session_ptr_(nullptr)
			, timeout_(timeout)
			, host_()
			, port_()
		{}

		virtual ~basic_client() = default;

	public:
		auto connect(const std::string& host, const std::string& port) -> awaitable<error_code>
		{
			host_ = host;
			port_ = port;

			auto socket = socket_t(io_context_);

			session_ptr_ = std::make_shared<Session>(std::move(socket));

			auto ec = co_await session_ptr_->async_connect(host, port);

			if (ec)
				co_return ec;

			co_return ec;
		}

		auto reconnect() -> awaitable<error_code>
		{
			co_return co_await connect(host_, port_);
		}

		template <typename Response, typename Request>
		auto send(std::shared_ptr<Request> req) -> awaitable<Response>
		{
			detail::flex_buffer<char> buffer{};

			if (!req->commit(buffer))
				co_return Response{};

			auto ec = co_await send(std::move(buffer));

			if (ec)
				co_return Response{};

			auto response = co_await session_ptr_->query<Response>();

			if (ec)
			{
				if (ec != boost::asio::error::eof)
				{
					XLOG_ERROR() << "on read some occur error - " << ec.what();
				}
				session_ptr_->shutdown();
				if (close_func_)
					close_func_();

				co_return Response{};
			}

			co_return response;
		}

		template <typename T>
		auto send(detail::flex_buffer<T> buffer) -> awaitable<error_code>
		{
			co_return co_await session_ptr_->async_send(std::move(buffer));
		}

		template <typename T>
		auto read(detail::flex_buffer<T>& buffer) -> awaitable<error_code>
		{
			co_return co_await session_ptr_->async_read_some(buffer);
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

	private:
		io_context& io_context_;

		std::shared_ptr<Session> session_ptr_;

		std::function<void()> close_func_;

		std::chrono::steady_clock::duration timeout_;

		std::string host_;

		std::string port_;
	};
} // namespace aquarius