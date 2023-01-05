#pragma once
#include "session.hpp"
#include "server/server.hpp"


namespace aquarius
{
	using tcp_no_ssl_server = srv::server;

#ifdef ENABLE_SSL
	using ssl_server = srv::server<tcp_ssl_session>
#endif
}