#pragma once
#include <aquarius/impl/connect.hpp>

namespace aquarius
{
	using nossl_tcp_connect = impl::connect<>;
	using nossl_http_connect = impl::connect<detail::http>;
	using ssl_tcp_connect = impl::connect<>;
	using ssl_http_connect = impl::connect<detail::http>;
} // namespace aquarius