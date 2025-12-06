#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/session.hpp>
#include <aquarius/ip/ssl_socket_adaptor.hpp>

namespace aquarius
{
	using http_ssl_client_session = session<protocol_tag::http, ssl_socket_adaptor<false, protocol_tag::http>>;

	using http_ssl_client = basic_client<http_ssl_client_session>;
} // namespace aquarius