#pragma once
#include <aquarius/basic_context.hpp>
#include <aquarius/detail/protocol.hpp>

namespace aquarius
{
	namespace tcp
	{
		class broadcast_context : public basic_context<context_kind::tcp_broadcast>
		{
			using base_type = basic_context<context_kind::tcp_broadcast>;

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
	} // namespace tcp
} // namespace aquarius