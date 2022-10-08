#pragma once
#include "session.hpp"
#include "server/basic_server.hpp"


namespace aquarius
{
	using server = srv::basic_server<tcp_no_ssl_session>;

#ifdef ENABLE_SSL
	using ssl_server = srv::basic_server<tcp_ssl_session>
#endif
}