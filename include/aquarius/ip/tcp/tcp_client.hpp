#pragma once 
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/tcp/tcp_client_session.hpp>

namespace aquarius
{
	using tcp_client = basic_client<tcp_client_session>;

	using ssl_tcp_client = basic_client<ssl_tcp_client_session>;
} // namespace aquarius