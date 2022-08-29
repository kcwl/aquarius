#pragma once
#include "connect/connect.hpp"
#include "connect/session.hpp"



namespace aquarius
{
	using tcp_no_ssl_session = aquarius::conn::basic_session<conn::tcp_session, socket_connect>;

	using http_no_sll_session = aquarius::conn::basic_session<conn::http_session, socket_connect>;

#if ENABLE_SSL
	using tcp_ssl_session = aquarius::conn::basic_session<conn::tcp_session, ssl_socket_connect>;

	using http_ssl_session = aquarius::conn::basic_session<conn::http_session, ssl_socket_connect>;
#endif
}