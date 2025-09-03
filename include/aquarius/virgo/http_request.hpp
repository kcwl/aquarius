#pragma once
#include <aquarius/virgo/basic_protocol_message.hpp>
#include <aquarius/virgo/http_header.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using http_request = basic_protocol_message<true, Header, http_header, Body>;
	}
} // namespace aquarius