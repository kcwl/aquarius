#include "subscriber.h"

namespace aquarius
{
	namespace serviced
	{
		subscriber::subscriber(std::size_t id)
			: id_(id)
		{}

		std::size_t subscriber::id() const
		{
			return id_;
		}

		void subscriber::id(std::size_t v)
		{
			id_ = v;
		}

		void subscriber::attach_session(const session_callback& cb)
		{
			cb_ = cb;
		}

		auto subscriber::invoke_session(flex_buffer& buffer) -> asio::awaitable<error_code>
		{
			co_return co_await cb_(buffer);
		}
	} // namespace serviced
} // namespace aquarius