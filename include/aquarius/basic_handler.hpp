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
		virtual auto visit(std::shared_ptr<Session> sessoin_ptr, std::shared_ptr<request_t> request_ptr)
			-> awaitable<error_code> = 0;

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

		auto session()
		{
			return session_ptr_;
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