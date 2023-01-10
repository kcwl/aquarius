#pragma once
#include <aquarius/impl/connect.hpp>

namespace aquarius
{
	using nossl_tcp_connect = impl::connect<nossl_socket>;
	using nossl_http_connect = impl::connect<nossl_socket, impl::http>;
	using ssl_tcp_connect = impl::connect<ssl_socket>;
	using ssl_http_connect = impl::connect<ssl_socket, impl::http>;
} // namespace aquarius