#pragma once
#include <aquarius/virgo/basic_http_message.hpp>

namespace aquarius
{
	namespace virgo
	{
		template <typename Header, typename Body>
		using http_response = basic_http_message<false, Header, Body>;
	}
} // namespace aquarius