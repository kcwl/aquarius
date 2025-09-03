#pragma once
#include <aquarius/virgo/basic_protocol_message.hpp>
#include <aquarius/virgo/http_header.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using response = basic_protocol_message<false, Header, http_header, Body>;
	}
} // namespace aquarius