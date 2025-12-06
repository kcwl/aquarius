#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/raw_socket_adaptor.hpp>
#include <aquarius/ip/session.hpp>

namespace aquarius
{
	using http_client_session = session<protocol_tag::http, raw_socket_adaptor<protocol_tag::http>>;

	using http_client = basic_client<http_client_session>;
} // namespace aquarius