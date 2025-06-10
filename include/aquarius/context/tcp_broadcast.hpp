#pragma once
#include <aquarius/context/basic_tcp_context.hpp>
#include <aquarius/flex_buffer.hpp>

namespace aquarius
{
	namespace context
	{
		class broadcast_context : public basic_tcp_context<context_kind::tcp_broadcast>
		{
			using base_type = basic_tcp_context<context_kind::tcp_broadcast>;

		public:
			using base_type::mode;

		public:
			broadcast_context()
				: base_type("broadcast context")
			{}

		public:
			template <typename... Args>
			void visit(flex_buffer buffer, std::size_t proto, Args&&...)
			{}
		};
	} // namespace context
} // namespace aquarius