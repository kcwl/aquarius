#pragma once
#include <aquarius/impl/server.hpp>
#include <aquarius/session.hpp>

namespace aquarius
{
	using tcp_no_ssl_server = impl::server;

#ifdef ENABLE_SSL
	using ssl_server = impl::server<tcp_ssl_session>
#endif
} // namespace aquarius