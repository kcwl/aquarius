#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/default_tcp_selector.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	using tcp_server_session = server_session<ip::default_tcp_selector>;

	using tcp_server = basic_server<tcp_server_session>;
} // namespace aquarius