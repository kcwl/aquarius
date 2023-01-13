#pragma once
#include <aquarius/impl/connect.hpp>
#include <aquarius/impl/socket.hpp>

namespace aquarius
{
	using nossl_tcp_connect = impl::connect<nossl_socket_t>;
	using nossl_http_connect = impl::connect<nossl_socket_t, detail::http>;
	using ssl_tcp_connect = impl::connect<ssl_socket_t>;
	using ssl_http_connect = impl::connect<ssl_socket_t, detail::http>;
} // namespace aquarius