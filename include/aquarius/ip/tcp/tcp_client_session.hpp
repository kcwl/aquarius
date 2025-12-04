#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/tcp/tcp.hpp>

namespace aquarius
{
	using tcp_client_session = basic_session<false, tcp>;
}