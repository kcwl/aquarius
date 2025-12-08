#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	using http_server_session = server_session<proto_tag::http>;

	using http_server = basic_server<http_server_session>;
} // namespace aquarius