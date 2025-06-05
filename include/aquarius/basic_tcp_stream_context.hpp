#pragma once
#include <aquarius/basic_context.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/tcp/context_router.hpp>

namespace aquarius
{
	template <typename Executor>
	class basic_tcp_stream_context : public basic_context<Executor, context_kind::tcp_stream>
	{
		using base_type = basic_context<Executor, context_kind::tcp_stream>;

	public:
		basic_tcp_stream_context()
			: base_type("tcp stream context")
		{}

	public:
		template <typename Request, typename Session, typename... Args>
		void visit(flex_buffer buffer, std::size_t proto, std::shared_ptr<Session> session, Args&&...)
		{
			auto request = std::make_shared<Request>();

			request->from_binary(buffer);

			boost::asio::post(this->get_executor(), [proto, buf = std::move(buffer), session]() mutable
							  { tcp::context_router<Session>::get_mutable_instance().invoke(proto, buf, session); });
		}
	};

} // namespace aquarius