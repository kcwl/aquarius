#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/client.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/connect.hpp>

namespace aquarius
{
	using no_ssl_tcp_connect = connect<connect_tcp>;

	using ssl_tcp_connect = connect<connect_tcp, ssl_socket>;

	using no_ssl_tcp_server = server<no_ssl_tcp_connect>;

	using ssl_tcp_server = server<ssl_tcp_connect>;

	using no_ssl_http_connect = connect<connect_http>;

	using ssl_http_connect = connect<connect_http, ssl_socket>;

	using no_ssl_http_server = server<no_ssl_http_connect>;

	using ssl_http_server = server<ssl_http_connect>;

	using no_ssl_tcp_client = client<no_ssl_tcp_connect>;

	using ssl_tcp_client = client<ssl_tcp_connect>;
} // namespace aquarius