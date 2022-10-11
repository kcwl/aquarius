#pragma once
#include "session/connect.hpp"
#include "session/session.hpp"



namespace aquarius
{
	using socket_connect = session::basic_connect<sock::socket<>>;

#if ENABLE_SSL
	using ssl_connect = session::basic_connect<sock::socket<ssl_socket>>;
#endif

	using tcp_no_ssl_session = aquarius::session::basic_session<session::tcp_session, socket_connect>;

	using http_no_sll_session = aquarius::session::basic_session<session::http_session, socket_connect>;

#if ENABLE_SSL
	using tcp_ssl_session = aquarius::conn::basic_session<conn::tcp_session, ssl_socket_connect>;

	using http_ssl_session = aquarius::conn::basic_session<conn::http_session, ssl_socket_connect>;
#endif
}