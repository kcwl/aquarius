#pragma once
#include <aquarius/detail/config.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/coroutine.hpp>
#include <memory>

namespace aquarius
{
	template <typename Session, typename Request, typename Response>
	class basic_handler
	{
	public:
		using request_t = Request;

		using response_t = Response;

	public:
		basic_handler(const std::string& name)
			: name_(name)
		{}

		virtual ~basic_handler() = default;

	public:
		auto visit(std::shared_ptr<Session> sessoin_ptr, std::shared_ptr<request_t> request_ptr, bool has_error = false)
			-> awaitable<void>
		{
			this->session_ptr_ = sessoin_ptr;
			this->request_ptr_ = request_ptr;

			error_code ec{};

			if (!has_error)
			{
				ec = co_await this->handle();
			}

			co_await send_response(ec);
		}

		std::string name() const
		{
			return name_;
		}

		std::shared_ptr<Request> request() const
		{
			return request_ptr_;
		}

		Response& response()
		{
			return response_;
		}

	protected:
		virtual auto handle() -> awaitable<error_code> = 0;

		virtual void make_response()
		{
			return;
		}

		auto session()
		{
			return session_ptr_;
		}

	private:
		virtual auto send_response(error_code ec) -> awaitable<void>
		{
			response().result(ec.value());

			flex_buffer buffer{};

			make_response();

			response().commit(buffer);

			co_await this->session()->async_send(buffer);
		}

	protected:
		std::shared_ptr<request_t> request_ptr_;

		response_t response_;

		std::shared_ptr<Session> session_ptr_;

	private:
		std::string name_;
	};
} // namespace aquarius

#define AQUARIUS_GLOBAL_STR_ID(request) #request