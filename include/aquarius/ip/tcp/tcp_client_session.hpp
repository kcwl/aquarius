#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/tcp/tcp_proc.hpp>

namespace aquarius
{
	using tcp_client_session = basic_session<false, tcp_proc>;
}