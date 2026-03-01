#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/protocol.hpp>
#include <aquarius/ip/session.hpp>
#include <aquarius/ip/ssl_socket_adaptor.hpp>

namespace aquarius
{
	using tcp_ssl_client_session = session<proto::tcp, ssl_socket_adaptor<false, proto::tcp>>;

	using tcp_ssl_client = basic_client<tcp_ssl_client_session>;
} // namespace aquarius