#pragma once
#include <aquarius/connect/basic_connect.hpp>
#include <aquarius/connect/no_ssl.hpp>
#include <aquarius/connect/ssl.hpp>

namespace aquarius
{
	using no_ssl_tcp_client_connect = basic_connect<tcp, std::false_type, no_ssl>;

	using ssl_tcp_client_connect = basic_connect<tcp, std::false_type, ssl>;

	using no_ssl_tcp_server_connect = basic_connect<tcp, std::true_type, no_ssl>;

	using ssl_tcp_server_connect = basic_connect<tcp, std::true_type, ssl>;
} // namespace aquarius
