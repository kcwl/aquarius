#pragma once
#include <aquarius/server/server.hpp>
#include <aquarius/server/tcp_connect.hpp>
#include <aquarius/core/defines.hpp>

namespace aquarius
{
	using nossl_tcp_connect = srv::tcp_connect<>;

	using ssl_tcp_connect = srv::tcp_connect<core::ssl_socket>;

	using no_ssl_server = srv::server<nossl_tcp_connect>;

	using ssl_server = srv::server<ssl_tcp_connect>;
} // namespace aquarius