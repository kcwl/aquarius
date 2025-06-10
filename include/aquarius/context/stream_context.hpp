#pragma once
#include <aquarius/context/basic_tcp_context.hpp>
#include <aquarius/context/detail/handler_router.hpp>
#include <aquarius/flex_buffer.hpp>

namespace aquarius
{
	namespace context
	{
		class stream_context : public basic_tcp_context<context_kind::stream>
		{
			using base_type = basic_tcp_context<context_kind::stream>;

		public:
			using base_type::mode;

		public:
			stream_context()
				: base_type("tcp stream context")
			{}

		public:
			template <typename Session, typename... Args>
			void visit(flex_buffer buffer, std::size_t proto, std::shared_ptr<Session> session, Args&&...)
			{
				post(session->get_executor(), [session, proto, buf = std::move(buffer)]() mutable
					 { detail::handler_router<Session>::get_mutable_instance().invoke(proto, buf, session); });
			}
		};
	} // namespace context
} // namespace aquarius