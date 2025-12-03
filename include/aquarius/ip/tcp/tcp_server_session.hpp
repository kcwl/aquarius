#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/tcp/tcp_proc.hpp>

namespace aquarius
{
	using tcp_server_session = basic_session<true, tcp_proc>;
} // namespace aquarius