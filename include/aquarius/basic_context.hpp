#pragma once
#include <aquarius.hpp>

namespace aquarius
{
	class context_base
	{
	public:
		virtual ~context_base() = default;
	};

	template <typename Protocol, typename... Args>
	class basic_protocol_context : public context_base
	{
	public:
		using function_type =
			std::function<asio::awaitable<error_code>(basic_protocol_context*, Protocol*, flex_buffer&, Args&&...)>;

	public:
		basic_protocol_context(function_type func)
			: func_(func)
		{}

		virtual ~basic_protocol_context() = default;

	public:
		auto complete(Protocol* proto, flex_buffer& buffer, Args&&... args) -> asio::awaitable<error_code>
		{
			co_return co_await func_(this, proto, buffer, std::forward<Args>(args)...);
		}

	private:
		function_type func_;
	};

	template <typename Handler, typename Protocol, typename... Args>
	class basic_context : public basic_protocol_context<Protocol, Args...>
	{
		using base_type = basic_protocol_context<Protocol, Args...>;

	public:
		basic_context()
			: base_type(&basic_context<Handler, Protocol, Args...>::do_complete)
		{}

		virtual ~basic_context() = default;

	public:
		static auto do_complete(base_type*, Protocol* proto, flex_buffer& buffer, Args&&... args)
			-> asio::awaitable<error_code>
		{
			co_return co_await proto->template handle_request<Handler>(buffer, std::forward<Args>(args)...);
		}
	};
} // namespace aquarius