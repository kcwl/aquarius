#pragma once
#include <aquarius/asio.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/ip/concept.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/module/http_config_module.hpp>
#include <aquarius/serialize/flex_buffer.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <functional>

namespace aquarius
{
	using namespace std::chrono_literals;

	template <typename Session, typename Executor = any_io_executor>
	class basic_client
	{
		using socket = Session::socket;

		using resolver = Session::resolver;

		using callback_t = std::function<void(std::shared_ptr<Session>)>;

		using executor_t = Executor;

	public:
		basic_client(io_context& context, std::chrono::milliseconds timeout)
			: basic_client(context.get_executor(), timeout)
		{}

		basic_client(const executor_t& e, std::chrono::milliseconds timeout)
			: executor_(e)
			, session_ptr_(nullptr)
			, close_func_()
			, accept_func_()
			, host_()
			, port_()
			, timeout_(timeout)
		{}

	public:
		executor_t get_executor() const
		{
			return executor_;
		}
		auto async_connect(const std::string& host, uint16_t port) -> awaitable<error_code>
		{
			session_ptr_ =
				std::make_shared<Session>(std::move(socket(this->get_executor())), 30ms, this->get_timeout());

			auto ec = co_await session_ptr_->async_connect(host, std::to_string(port));

			if (!make_error(ec))
			{
				host_ = host;
				port_ = port;

				accept_invoke();
			}

			co_return ec;
		}

		auto reconnect() -> awaitable<error_code>
		{
			co_return co_await async_connect(host_, port_);
		}

		auto async_send(flex_buffer&& buffer) -> awaitable<error_code>
		{
			co_return co_await session_ptr_->async_send(std::move(buffer));
		}

		std::string remote_address() const
		{
			return session_ptr_->remote_address();
		}

		int remote_port() const
		{
			return session_ptr_->remote_port();
		}

		void close()
		{
			session_ptr_->close();

			std::shared_ptr<Session>().swap(session_ptr_);
		}

		std::chrono::milliseconds get_timeout() const
		{
			return timeout_;
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

		template <typename Response, typename Request>
		requires(is_message_type<Request>::value && is_message_type<Response>::value)
		auto async_send(std::shared_ptr<Request> req) -> awaitable<Response>
		{
			flex_buffer buffer{};
			buffer.commit(sizeof(uint32_t));

			req->commit(buffer);

			auto size = buffer.size() - sizeof(uint32_t);

			buffer.pubseekpos(0, std::ios::in);

			buffer.sputn((char*)&size, sizeof(uint32_t));

			buffer.pubseekoff(size, std::ios::cur, std::ios::in);

			auto ec = co_await async_send(std::move(buffer));

			Response resp{};

			if (make_error(ec))
			{
				co_return resp;
			}

			auto resp_buffer = co_await session_ptr_->query(req->header().sequence(), ec);

			if (make_error(ec))
			{
				co_return resp;
			}

			resp.consume(resp_buffer);

			co_return resp;
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
		executor_t executor_;

		std::shared_ptr<Session> session_ptr_;

		std::function<void(std::shared_ptr<Session>)> close_func_;

		std::function<void(std::shared_ptr<Session>)> accept_func_;

		std::string host_;

		uint16_t port_;

		std::chrono::milliseconds timeout_;
	};
} // namespace aquarius