#pragma once
#include "session/connect.hpp"
#include "session/session.hpp"



namespace aquarius
{
	using socket_connect = session::connector<sock::socket<>>;

#if ENABLE_SSL
	using ssl_connect = session::connector<sock::socket<ssl_socket>>;
#endif

	using tcp_no_ssl_session = aquarius::session::session<session::tcp_session, socket_connect>;

	using http_no_sll_session = aquarius::session::session<session::http_session, socket_connect>;

#if ENABLE_SSL
	using tcp_ssl_session = aquarius::conn::session<conn::tcp_session, ssl_socket_connect>;

	using http_ssl_session = aquarius::conn::session<conn::http_session, ssl_socket_connect>;
#endif
}