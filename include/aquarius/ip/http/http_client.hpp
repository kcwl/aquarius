#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/adaptor/raw_socket_adaptor.hpp>
#include <aquarius/ip/default_selector.hpp>

namespace aquarius
{
	using http_client_session = basic_session<http_protocol, raw_socket_adaptor, http_selector>;

	using http_client = basic_client<http_client_session>;
} // namespace aquarius