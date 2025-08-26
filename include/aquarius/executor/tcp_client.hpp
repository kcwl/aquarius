#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/executor/tcp_client_session.hpp>

namespace aquarius
{
	using tcp_client = basic_client<tcp_client_session>;

	using ssl_tcp_client = basic_client<ssl_tcp_client_session>;

	using tcp_executor_client = basic_exector_client<tcp_client_session>;

	using ssl_tcp_executor_client = basic_exector_client<ssl_tcp_client_session>;
} // namespace aquarius