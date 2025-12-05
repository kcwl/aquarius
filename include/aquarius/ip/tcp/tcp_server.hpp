
#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	using tcp_server = basic_server<server_session<protocol_tag::tcp>>;
} // namespace aquarius