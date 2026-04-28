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
	class basic_client
	{
		using socket = Session::socket;

		using resolver = Session::resolver;

		using callback_t = std::function<void(std::shared_ptr<Session>)>;

		using executor_t = Executor;

		struct awaitable
		{
			basic_client* self;

			std::size_t src;

			bool await_ready()
			{
				return self->buffers_.find(src) != self->buffers_.end();
			}

			void await_suspend(std::coroutine_handle<void> h)
			{
				asio::co_spawn(
					co_await asio::this_coro::executor,
					[handle = std::move(h)]() -> asio::awaitable<void> { handle.resume(); }, asio::detached);
			}

			void await_resume()
			{}

			awaitable get_return_objec()
			{
				return *this;
			}
		};

		struct callback
		{
			using func_t = std::function<void(flex_buffer)>;

			flex_buffer buffer;
			func_t func;
		};

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
			}

			co_return ec;
		}

		auto reconnect() -> asio::awaitable<error_code>
		{
			co_return co_await async_connect(host_, port_);
		}

		auto async_send(flex_buffer&& buffer) -> asio::awaitable<error_code>
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
		auto async_call(std::shared_ptr<Request> req) -> asio::awaitable<Response>
		{
			flex_buffer buffer{};

			req->commit(buffer);

			auto ec = co_await async_send(std::move(buffer));

			Response resp{};

			if (make_error(ec))
			{
				co_return resp;
			}

			auto result = co_await session_ptr_->query();

			if (!result.has_value())
			{
				if (make_error(result.error()))
				{
					co_return resp;
				}
			}

			resp.consume(*result);

			co_return resp;
		}

		template <typename Request>
		auto async_call(std::shared_ptr<Request> req) -> asio::awaitable<bool>
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

	private:
		bool regist_resp_buffer(std::size_t src, flex_buffer buffer)
		{
			std::lock_guard lk(buf_mutex_);

			auto iter = buffers_.find(src);
			if (iter == buffers_.end())
			{
				return false;
			}

			iter->second.buffer = std::move(buffer);

			return true;
		}

		void regist_resp_func(std::size_t src, const callback::func_t& func)
		{
			std::lock_guard lk(buf_mutex_);

			auto iter = buffers_.find(src);
			if (iter != buffers_.end())
			{
				return;
			}

			auto ptr = std::make_shared<callback>();

			ptr->func = func;
		}

		auto wait_buffer(std::size_t src) -> awaitable
		{
			co_return awaitable(this, src);
		}

		void make_response(std::size_t src)
		{
			std::lock_guard lk(buf_mutex_);

			auto iter = buffers_.find(src);

			if (iter == buffers_.end())
			{
				return;
			}

			iter->second.func(iter->second.buffer);

			buffers_.erase(iter);
		}

	private:
		executor_t executor_;

		std::shared_ptr<Session> session_ptr_;

		std::function<void(std::shared_ptr<Session>)> close_func_;

		std::function<void(std::shared_ptr<Session>)> accept_func_;

		std::string host_;

		uint16_t port_;

		std::chrono::milliseconds timeout_;

		std::mutex buf_mutex_;

		std::map<std::size_t, std::shared_ptr<callback>> buffers_;
	};
} // namespace aquarius