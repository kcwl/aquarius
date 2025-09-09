#pragma once
#include <aquarius/virgo/basic_tcp_message.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using tcp_response = basic_tcp_message<false, Header, Body>;
	}
} // namespace aquarius