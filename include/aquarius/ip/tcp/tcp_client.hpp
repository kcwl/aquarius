#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/adaptor/raw_socket_adaptor.hpp>

namespace aquarius
{
	using tcp_client_session = basic_session<tcp_protocol, raw_socket_adaptor>;

	using tcp_client = basic_client<tcp_client_session>;
} // namespace aquarius