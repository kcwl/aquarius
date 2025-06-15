#pragma once
#include <boost/asio.hpp>
#include <memory>

namespace aquarius
{
	template <typename RPC, typename E>
	class basic_handler
	{
		using request_type = typename RPC::request_type;

		using response_type = typename RPC::response_type;

	public:
		basic_handler(const std::string& name)
			: name_(name)
		{

		}

		virtual ~basic_handler() = default;

	public:
		auto visit(std::shared_ptr<RPC> message) -> awaitable<E>
		{
			message_ = message;

			co_return co_await this->handle();
		}

		request_type request() const
		{
			return message_->request();
		}

		response_type& response()
		{
			return message_->response();
		}

	protected:
		virtual auto handle() -> awaitable<E> = 0;

	protected:
		std::shared_ptr<RPC> message_;

		std::string name_;
	};
} // namespace aquarius