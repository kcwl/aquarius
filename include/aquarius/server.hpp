#pragma once
#include <aquarius/impl/server.hpp>
#include <aquarius/connect.hpp>

namespace aquarius
{
	using no_ssl_server = impl::server<nossl_tcp_connect>;

	using ssl_server = impl::server<ssl_tcp_connect>;
} // namespace aquarius