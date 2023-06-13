#pragma once
#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <aquarius/client.hpp>
#include <aquarius/context.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/server.hpp>
#include <aquarius/tcp_connect.hpp>

namespace aquarius
{
	using nossl_tcp_connect = tcp_connect<>;

	using ssl_tcp_connect = tcp_connect<detail::ssl_socket>;

	using no_ssl_server = server<nossl_tcp_connect>;

	using ssl_server = server<ssl_tcp_connect>;
} // namespace aquarius