#pragma once
#include <aquarius/impl/client_router.hpp>

namespace aquarius
{
	struct future_flow
	{
		template <typename Session, typename BuffSequence, typename Header>
		static void recv(std::shared_ptr<Session>, BuffSequence buffer, Header h)
		{
			impl::client_router::get_mutable_instance().invoke(h.rpc_id(), std::move(buffer));
		}
	};
} // namespace aquarius