#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/http_config_module.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <functional>

namespace aquarius
{
	using namespace std::chrono_literals;

	class client_base
	{
	public:
		explicit client_base(std::chrono::milliseconds timeout)
			: host_()
			, port_()
			, timeout_(timeout)
		{}

		virtual ~client_base() = default;

	public:
		virtual auto async_connect(const std::string& host, const std::string& port) -> awaitable<error_code> = 0;

		auto reconnect() -> awaitable<error_code>
		{
			co_return co_await async_connect(host_, port_);
		}

		virtual auto async_send_buffer(flex_buffer&&) -> awaitable<error_code> = 0;

		virtual auto query(uint32_t, error_code&) -> awaitable<flex_buffer> = 0;

		template <typename Response, typename Request>
		requires is_message_type<Request>::value && is_message_type<Response>::value
		auto async_send(std::shared_ptr<Request> req) -> awaitable<Response>
		{
			flex_buffer buffer{};

			req->commit(buffer);

			auto ec = co_await async_send_buffer(std::move(buffer));

			Response resp{};

			if (make_error(ec))
			{
				co_return resp;
			}

			auto resp_buffer = co_await query(req->seq_number(), ec);

			if (make_error(ec))
			{
				co_return resp;
			}

			resp.consume_header(resp_buffer);
			resp.consume_body(resp_buffer);

			co_return resp;
		}

		virtual std::string remote_address() const = 0;

		virtual int remote_port() const = 0;

		virtual void close() = 0;

		virtual bool make_error(error_code& ec) = 0;

		std::chrono::milliseconds get_timeout() const
		{
			return timeout_;
		}

	protected:
		void set_host_and_port(const std::string& host, const std::string& port)
		{
			host_ = host;
			port_ = port;
		}

	private:
		std::string host_;

		std::string port_;

		std::chrono::milliseconds timeout_;
	};

	template <typename Session, typename Executor = any_io_executor>
	class basic_client : public client_base
	{
		using socket = Session::socket;

		using resolver = Session::resolver;

		using base = client_base;

		using callback_t = std::function<void(std::shared_ptr<Session>)>;

		using executor_t = Executor;

	public:
		basic_client(io_context& context, std::chrono::milliseconds timeout)
			: basic_client(context.get_executor(), timeout)
		{}

		basic_client(const executor_t& e, std::chrono::milliseconds timeout)
			: base(timeout)
			, executor_(e)
			, session_ptr_(nullptr)
			, close_func_()
			, accept_func_()
		{}

	public:
		Executor get_executor() const
		{
			return executor_;
		}
		virtual auto async_connect(const std::string& host, const std::string& port) -> awaitable<error_code> override
		{
			this->set_host_and_port(host, port);

			session_ptr_ =
				std::make_shared<Session>(std::move(socket(this->get_executor())), 30ms, this->get_timeout());

			auto ec = co_await session_ptr_->async_connect(host, port);

			if (make_error(ec))
				co_return ec;

			accept_invoke();

			co_return ec;
		}

		virtual auto async_send_buffer(flex_buffer&& buffer) -> awaitable<error_code> override
		{
			co_return co_await session_ptr_->async_send(std::move(buffer));
		}

		virtual auto query(uint32_t seq, error_code& ec) -> awaitable<flex_buffer> override
		{
			co_return co_await session_ptr_->query(seq, ec);
		}

		virtual std::string remote_address() const override
		{
			return session_ptr_->remote_address();
		}

		virtual int remote_port() const override
		{
			return session_ptr_->remote_port();
		}

		virtual void close() override
		{
			session_ptr_->close();

			std::shared_ptr<Session>().swap(session_ptr_);
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

	protected:
		bool make_error(error_code& ec)
		{
			if (!ec)
				return false;

			if (ec != boost::asio::error::eof)
			{
				XLOG_ERROR() << "on read some occur error - " << ec.what();
			}

			close_invoke();

			session_ptr_->close();

			return true;
		}

	private:
		Executor executor_;

		std::shared_ptr<Session> session_ptr_;

		std::function<void(std::shared_ptr<Session>)> close_func_;

		std::function<void(std::shared_ptr<Session>)> accept_func_;
	};
} // namespace aquarius