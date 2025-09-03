#pragma once
#include <aquarius/virgo/basic_protocol_message.hpp>
#include <aquarius/virgo/tcp_header.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using tcp_request = basic_protocol_message<true, Header, tcp_header, Body>;
	}
} // namespace aquarius