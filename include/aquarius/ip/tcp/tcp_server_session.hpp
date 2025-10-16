#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/tcp/tcp.hpp>

namespace aquarius
{
	using tcp_server_session = basic_session<true, tcp>;
} // namespace aquarius