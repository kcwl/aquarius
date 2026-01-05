#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/default_http_selector.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	using http_server_session = server_session<ip::default_http_selector>;

	using http_server = basic_server<http_server_session>;
} // namespace aquarius