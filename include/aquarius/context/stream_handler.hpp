#pragma once
#include <aquarius/basic_handler.hpp>

namespace aquarius
{
	template <typename RPC, typename E>
	class stream_handler : public basic_handler<RPC, E>
	{
		using base_type = basic_handler<RPC, E>;

	public:
		stream_handler(const std::string& name)
			: base_type(name)
		{}

	public:
		auto visit(std::shared_ptr<RPC> message) -> awaitable<E>
		{
			auto result = co_await base_type::visit(message);

			make_response(result);

			co_return result;
		}

	private:
		void make_response(E result)
		{
			this->response().header()->set_result(static_cast<int64_t>(result));
		}
	};
} // namespace aquarius