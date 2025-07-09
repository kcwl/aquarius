#pragma once
#include <aquarius/ip/detail/tcp.hpp>
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_server.hpp>

namespace aquarius
{
	namespace ip
	{
		using client_tcp_protocol = detail::basic_tcp<false>;

		using server_tcp_protocol = detail::basic_tcp<true>;

		using tcp_client = basic_client<client_tcp_protocol>;

		using tcp_server = basic_server<server_tcp_protocol>;
	} // namespace ip
} // namespace aquarius