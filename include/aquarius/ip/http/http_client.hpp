#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/adaptor/raw_socket_adaptor.hpp>
#include <aquarius/ip/default_selector.hpp>
#include <aquarius/ip/session.hpp>

namespace aquarius
{
	using http_client_session = session<http_protocol, raw_socket_adaptor, ip::http_selector>;

	using http_client = basic_client<http_client_session>;
} // namespace aquarius