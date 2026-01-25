#pragma once
#include <aquarius/ip/server_session.hpp>
#include <aquarius/ip/tcp/tcp_server.hpp>

namespace aquarius
{
	namespace serviced
	{
		// using channel_server_session = server_session<channel_selector>;

		// using server = aquarius::basic_server<channel_server_session>;
		using server = aquarius::tcp_server;
	} // namespace serviced
} // namespace aquarius
