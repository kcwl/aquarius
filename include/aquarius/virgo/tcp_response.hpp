#pragma once
#include <aquarius/virgo/basic_protocol_message.hpp>
#include <aquarius/virgo/tcp_header.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using tcp_response = basic_protocol_message<false, Header, tcp_header<false>, Body>;
	}
} // namespace aquarius