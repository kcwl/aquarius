#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>

namespace aquarius
{
	template <detail::string_literal Router, typename Body>
	using http_request = basic_http_protocol<true, Router, Body>;
} // namespace aquarius