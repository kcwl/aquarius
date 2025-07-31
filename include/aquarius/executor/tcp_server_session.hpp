#pragma once
#include <aquarius/basic_tcp_session.hpp>
#include <aquarius_protocol.hpp>

namespace aquarius
{
	using tcp_server_session = basic_tcp_session<true, layer_tcp_header>;
}