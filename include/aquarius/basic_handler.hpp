#pragma once
#include <boost/asio.hpp>
#include <memory>

namespace aquarius
{
	template <typename Request, typename Response, typename E>
	class basic_handler
	{
	public:
		basic_handler(const std::string& name)
			: name_(name)
		{

		}

		virtual ~basic_handler() = default;

	public:
		auto visit(std::shared_ptr<Request> message) -> awaitable<E>
		{
			request_ = message;

			co_return co_await this->handle();
		}

		std::shared_ptr<Request> request() const
		{
			return request_;
		}

		Response& response()
		{
			return response_;
		}

		void set_rpc_id(std::size_t rpc_id)
		{
			rpc_id_ = rpc_id;
		}

		std::size_t rpc_id() const
		{
			return rpc_id_;
		}

	protected:
		virtual auto handle() -> awaitable<E> = 0;

	protected:
		std::shared_ptr<Request> request_;

		Response response_;

		std::string name_;

		std::size_t rpc_id_;
	};
} // namespace aquarius