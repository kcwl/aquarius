#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/client_session.hpp>

namespace aquarius
{
	using http_client = basic_client<client_session<protocol_tag::http>>;
} // namespace aquarius