#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/http_config_module.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/header_fields.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <functional>

namespace aquarius
{
	using namespace std::chrono_literals;

	template <typename Session, typename Executor = any_io_executor>
	class basic_client : public std::enable_shared_from_this<basic_client<Session>>
	{
		using socket = Session::socket;

		using resolver = Session::resolver;

	public:
		explicit basic_client(io_context& context, std::chrono::milliseconds timeout = 1000ms)
			: basic_client(context.get_executor(), timeout)
		{}

		basic_client(const Executor& executor, std::chrono::milliseconds timeout)
			: executor_(executor)
			, host_()
			, port_()
			, session_ptr_(nullptr)
			, close_func_()
			, accept_func_()
			, timeout_(timeout)
		{
			[[maybe_unused]] static aquarius::logger __auto_init_log;
		}

		virtual ~basic_client() = default;

	public:
		auto async_connect(const std::string& host, const std::string& port) -> awaitable<error_code>
		{
			host_ = host;
			port_ = port;

			session_ptr_ = std::make_shared<Session>(std::move(socket(executor_)), 30ms, timeout_);

			auto ec = co_await session_ptr_->async_connect(host_, port_);

			if (make_error(ec))
				co_return ec;

			accept_invoke();

			co_return ec;
		}

		auto reconnect() -> awaitable<error_code>
		{
			co_return co_await async_connect(host_, port_);
		}

		template <typename Response, typename Request>
		auto async_send(std::shared_ptr<Request> req) -> awaitable<Response>
		{
			flex_buffer buffer{};

			// req->seq_number(static_cast<uint32_t>(detail::uuid_generator()()));

			req->commit(buffer);

			// error_code ec{};

			virgo::header_fields hf{};

			auto ec = co_await session_ptr_->async_send(buffer);

			Response resp{};

			if (make_error(ec))
			{
				co_return resp;
			}

			co_await session_ptr_->query(resp, req->seq_number(), hf, ec);

			make_error(ec);

			co_return resp;
		}

		std::string remote_address() const
		{
			return session_ptr_->remote_address();
		}

		int remote_port() const
		{
			return session_ptr_->remote_port();
		}

		template <typename Func>
		void set_close_func(Func&& f)
		{
			close_func_ = std::forward<Func>(f);
		}

		void close_invoke()
		{
			if (!close_func_)
				return;

			close_func_(session_ptr_);
		}

		template <typename Func>
		void set_accept_func(Func&& f)
		{
			accept_func_ = std::forward<Func>(f);
		}

		void accept_invoke()
		{
			if (!accept_func_)
				return;

			accept_func_(session_ptr_);
		}

		void close()
		{
			session_ptr_->close();

			std::shared_ptr<Session>().swap(session_ptr_);
		}

	private:
		bool make_error(error_code& ec)
		{
			if (!ec)
				return false;

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "on read some occur error - " << ec.what();
			}

			session_ptr_->shutdown();

			close_invoke();

			return true;
		}

	private:
		Executor executor_;

		std::string host_;

		std::string port_;

		std::shared_ptr<Session> session_ptr_;

		std::function<void(std::shared_ptr<Session>)> close_func_;

		std::function<void(std::shared_ptr<Session>)> accept_func_;

		std::chrono::milliseconds timeout_;
	};
} // namespace aquarius