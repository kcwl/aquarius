#pragma once
#include <aquarius/basic_tcp_session.hpp>
#include <aquarius_protocol.hpp>

namespace aquarius
{
	using tcp_client_session = basic_tcp_session<false, layer_tcp_header>;
}