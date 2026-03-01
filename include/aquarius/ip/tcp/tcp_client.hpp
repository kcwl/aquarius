#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/default_selector.hpp>
#include <aquarius/ip/raw_socket_adaptor.hpp>
#include <aquarius/ip/session.hpp>

namespace aquarius
{
	using tcp_client_session = session<proto::tcp, raw_socket_adaptor, ip::tcp_selector>;

	using tcp_client = basic_client<tcp_client_session>;
} // namespace aquarius