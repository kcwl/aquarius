#pragma once
#include <aquarius/detail/impl/context.hpp>
#include <aquarius/detail/impl/handler_router.hpp>

namespace aquarius
{
	struct stream_flow
	{
		template <typename Session, typename BuffSequence, typename Header>
		static void recv(std::shared_ptr<Session> session_ptr, BuffSequence buffer, Header h)
		{
			if (!session_ptr)
				return;

			co_spawn(
				session_ptr->get_executor(),
				[buffer = std::move(buffer), session_ptr, h = std::move(h)]() mutable -> awaitable<void>
				{
					detail::impl::handler_router<Session>::get_mutable_instance().invoke(h.rpc_id(), session_ptr,
																				 std::move(buffer), h);
					co_return;
				},
				detached);
		}
	};
} // namespace aquarius