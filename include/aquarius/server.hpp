#pragma once
#include <aquarius/connect.hpp>
#include <aquarius/impl/server.hpp>

namespace aquarius
{
	using no_ssl_server = impl::server<nossl_tcp_connect>;

	using ssl_server = impl::server<ssl_tcp_connect>;
} // namespace aquarius