#pragma once
#include <aquarius/virgo/basic_tcp_message.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using tcp_request = basic_tcp_message<true, Header, Body>;
	}
} // namespace aquarius