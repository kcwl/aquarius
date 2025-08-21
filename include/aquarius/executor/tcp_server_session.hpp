#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/protocol/tcp.hpp>

namespace aquarius
{
	using tcp_server_session = basic_session<true, tcp>;
}