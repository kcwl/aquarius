#pragma once
#include <aquarius/virgo/basic_protocol_message.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using tcp_response = basic_protocol_message<false, Header, Body>;
	}
} // namespace aquarius