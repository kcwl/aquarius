#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/protocol/tcp.hpp>

namespace aquarius
{
	using tcp_client_session = basic_session<false, tcp>;

	using ssl_tcp_client_session = basic_session<false, tcp, detail::ssl_context_factory<false>>;
}