#pragma once
#include <aquarius/basic_context.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/tcp/handler_router.hpp>

namespace aquarius
{
	namespace tcp
	{
		class stream_context : public basic_context<context_kind::stream>
		{
			using base_type = basic_context<context_kind::stream>;
		public:
			using base_type::mode;

		public:
			stream_context()
				: base_type("tcp stream context")
			{}

		public:
			template <typename Request, typename Session, typename... Args>
			void visit(flex_buffer buffer, std::size_t proto, std::shared_ptr<Session> session, Args&&...)
			{
				auto request = std::make_shared<Request>();

				request->from_binary(buffer);

				boost::asio::post(
					session->get_executor(), [proto, buf = std::move(buffer), session]() mutable
					{ tcp::handler_router<Session>::get_mutable_instance().invoke(proto, buf, session); });
			}
		};
	} // namespace tcp
} // namespace aquarius