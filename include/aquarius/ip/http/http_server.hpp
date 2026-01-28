#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	using http_server_session = custom_server_session<proto::http, ip::http_selector>;

	using http_server = basic_server<http_server_session>;
} // namespace aquarius