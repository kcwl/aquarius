#pragma once
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_header.hpp>

namespace aquarius
{
	template <typename Body>
	using http_response = basic_http_protocol<false, "empty", Body>;
} // namespace aquarius