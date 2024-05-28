#pragma once
#include <aquarius/connect/connect.hpp>
#include <aquarius/connect/no_ssl.hpp>
#include <aquarius/connect/ssl.hpp>

namespace aquarius
{
	using no_ssl_tcp_client_connect = connect<tcp, std::false_type, no_ssl>;

	using ssl_tcp_client_connect = connect<tcp, std::false_type, ssl>;

	using no_ssl_tcp_server_connect = connect<tcp, std::true_type, no_ssl>;

	using ssl_tcp_server_connect = connect<tcp, std::true_type, ssl>;
} // namespace aquarius
