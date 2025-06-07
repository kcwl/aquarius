#pragma once
#include <boost/asio.hpp>

namespace aquarius
{
	template <typename T, typename E>
	class basic_handler
	{
	public:
		basic_handler(const std::string& name)
			: name_(name)
		{

		}

		virtual ~basic_handler() = default;

	public:
		auto visit(T message) -> boost::asio::awaitable<E>
		{
			message_ = message;

			co_return co_await this->handle();
		}

		T message() const
		{
			return message_;
		}

	protected:
		virtual auto handle() -> boost::asio::awaitable<E> = 0;

	protected:
		T message_;

		std::string name_;
	};
} // namespace aquarius