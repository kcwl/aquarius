#pragma once
#include <aquarius/detail/asio.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/logger.hpp>
#include <aquarius/virgo/http_version.hpp>
#include <functional>

namespace aquarius
{
	using namespace std::chrono_literals;

	template <typename Session, typename Executor = asio::any_io_executor>
	class basic_client : public std::enable_shared_from_this<basic_client<Session, Executor>>
	{
		using socket = Session::socket;

		using resolver = Session::resolver;

		using callback_t = std::function<void(std::shared_ptr<Session>)>;

		using executor_t = Executor;

	public:
		basic_client(asio::io_context& context, std::chrono::milliseconds timeout)
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
		auto async_connect(const std::string& host, uint16_t port) -> asio::awaitable<error_code>
		{
			session_ptr_ = std::make_shared<Session>(std::move(socket(this->get_executor())), this->get_timeout());

			auto ec = co_await session_ptr_->async_connect(host, std::to_string(port));

			if (!make_error(ec))
			{
				host_ = host;
				port_ = port;

				accept_invoke();

				auto self = this->shared_from_this();

				asio::co_spawn(
					executor_,
					[&, this, self]() -> asio::awaitable<void>
					{
						for (;;)
						{
							auto ec = co_await session_ptr_->query();

							if (ec)
							{
								break;
							}
						}
					},
					asio::detached);
			}

			co_return ec;
		}

		auto async_send(flex_buffer& buffer) -> asio::awaitable<error_code>
		{
			co_return co_await session_ptr_->async_send(buffer);
		}

		void close()
		{
			session_ptr_->close();
			session_ptr_.reset();
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
		auto async_call(std::shared_ptr<Request> req) -> asio::awaitable<Response>
		{
			Response resp{};

			auto f = [&] (flex_buffer& buffer)->asio::awaitable<void> { resp.consume(buffer); co_return; };

			error_code ec{};

			auto src = co_await session_ptr_->send_request(req, f, ec);

			if (make_error(ec))
			{
				co_return resp;
			}

			co_await session_ptr_->wait(src);

			make_error(ec);

			co_return resp;
		}

		template<typename Func, typename ConstBufferSequence>
		auto async_call_buffer(ConstBufferSequence&& req, Func&& f) -> asio::awaitable<error_code>
		{
			error_code ec{};

			auto src = co_await session_ptr_->send_buffer(std::forward<ConstBufferSequence>(req), std::forward<Func>(f), ec);

			co_await session_ptr_->wait(src);

			make_error(ec);

			co_return ec;
		}

		template <typename Request>
		auto async_send(std::shared_ptr<Request> req) -> asio::awaitable<bool>
		{
			flex_buffer buffer{};

			req->commit(buffer);

			auto ec = co_await async_send(std::move(buffer));

			make_error(ec);

			co_return !ec;
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

	protected:
		executor_t executor_;

		std::shared_ptr<Session> session_ptr_;

		std::function<void(std::shared_ptr<Session>)> close_func_;

		std::function<void(std::shared_ptr<Session>)> accept_func_;

		std::string host_;

		uint16_t port_;

		std::chrono::milliseconds timeout_;
	};
} // namespace aquarius