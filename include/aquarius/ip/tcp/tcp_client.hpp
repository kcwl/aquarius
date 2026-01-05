#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/default_tcp_selector.hpp>
#include <aquarius/ip/raw_socket_adaptor.hpp>
#include <aquarius/ip/session.hpp>

namespace aquarius
{
	using tcp_client_session = session<ip::default_tcp_selector, raw_socket_adaptor<proto_tag::tcp>>;

	using tcp_client = basic_client<tcp_client_session>;
} // namespace aquarius