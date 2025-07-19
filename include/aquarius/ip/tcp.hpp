#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/basic_tcp.hpp>
#include <aquarius/ip/flow.hpp>

namespace aquarius
{
	namespace ip
	{
		using client_tcp_protocol = basic_tcp<tcp_stream_flow<false>>;

		using server_tcp_protocol = basic_tcp<tcp_stream_flow<true>>;

		using tcp_client = basic_client<client_tcp_protocol>;

		using tcp_server = basic_server<server_tcp_protocol>;
	} // namespace ip
} // namespace aquarius