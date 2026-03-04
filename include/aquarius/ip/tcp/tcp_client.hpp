#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/adaptor/raw_socket_adaptor.hpp>
#include <aquarius/ip/default_selector.hpp>
#include <aquarius/ip/session.hpp>

namespace aquarius
{
	using tcp_client_session = session<tcp_protocol, raw_socket_adaptor, tcp_selector>;

	using tcp_client = basic_client<tcp_client_session>;
} // namespace aquarius