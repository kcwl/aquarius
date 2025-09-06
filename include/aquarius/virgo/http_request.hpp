#pragma once
#include <aquarius/virgo/basic_http_message.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using http_request = basic_http_message<true, Header, Body>;
	}
} // namespace aquarius