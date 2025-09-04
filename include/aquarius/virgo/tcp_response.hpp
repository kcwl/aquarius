#pragma once
#include <aquarius/virgo/basic_protocol_message.hpp>
#include <aquarius/virgo/tcp_header.hpp>
#include <aquarius/detail/binary.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using tcp_response = basic_protocol_message<false, Header, tcp_header, Body, detail::binary_parse>;
	}
} // namespace aquarius