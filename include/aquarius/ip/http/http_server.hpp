#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/default_selector.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	using http_server_session = server_session<http_protocol, ip::http_selector>;

	using http_server = basic_server<http_server_session>;
} // namespace aquarius