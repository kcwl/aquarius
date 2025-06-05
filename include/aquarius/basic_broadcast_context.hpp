#pragma once
#include <aquarius/basic_context.hpp>
#include <aquarius/detail/protocol.hpp>

namespace aquarius
{
	template <typename Executor>
	class basic_broadcast_context : public basic_context<Executor, context_kind::tcp_broadcast>
	{
		using base_type = basic_context<Executor, context_kind::tcp_broadcast>;

	public:
		basic_broadcast_context()
			: base_type("broadcast context")
		{}

	public:
		template <typename... Args>
		void visit(flex_buffer buffer, std::size_t proto, Args&&...)
		{}
	};

} // namespace aquarius