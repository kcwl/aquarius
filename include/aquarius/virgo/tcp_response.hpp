#pragma once
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/virgo/tcp_header.hpp>

namespace aquarius
{
	template <detail::string_literal Router, typename Body>
	using tcp_response = basic_tcp_protocol<false, Router, tcp_header, Body>;
} // namespace aquarius