
#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/tcp/tcp_server_session.hpp>

namespace aquarius
{
	using tcp_server = basic_server<tcp_server_session>;
} // namespace aquarius