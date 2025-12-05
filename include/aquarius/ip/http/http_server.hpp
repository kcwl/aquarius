#pragma once
#include <aquarius/ip/server_session.hpp>
#include <aquarius/basic_server.hpp>

namespace aquarius
{
	using http_server = basic_server<server_session<protocol_tag::http>>;
}