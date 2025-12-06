#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/ssl_server_session.hpp>

namespace aquarius
{
	using tcp_ssl_server_session = ssl_server_session<proto_tag::tcp>;

	using tcp_ssl_server = basic_server<tcp_ssl_server_session>;
} // namespace aquarius