#pragma once
#include <aquarius/impl/context.hpp>

namespace aquarius
{
	struct raw_buffer_flow
	{
		template <typename Session, typename BuffSequence, typename Header>
		static void recv(std::shared_ptr<Session> session_ptr, BuffSequence buffer, Header h)
		{
			//impl::transfer<Session>()(session_ptr, std::move(buffer), h);
		}
	};
} // namespace aquarius