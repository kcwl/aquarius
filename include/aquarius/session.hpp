#pragma once
#include <aquarius/impl/connect.hpp>
#include <aquarius/impl/session.hpp>

namespace aquarius
{
	using socket_connect = impl::connector<impl::socket<>>;

#if ENABLE_SSL
	using ssl_connect = impl::connector<sock::socket<ssl_socket>>;
#endif

	// using tcp_no_ssl_session = aquarius::session::session<socket_connect>;

	// using http_no_sll_session = aquarius::session::session<session::http_session, socket_connect>;

#if ENABLE_SSL
	using tcp_ssl_session = aquarius::conn::impl<conn::tcp_session, ssl_socket_connect>;

	using http_ssl_session = aquarius::conn::impl<conn::http_session, ssl_socket_connect>;
#endif
} // namespace aquarius