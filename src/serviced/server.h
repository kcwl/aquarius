#pragma once
#include "channel_selector.hpp"
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	namespace serviced
	{
		using channel_server_session = server_session<channel_selector>;

		using server = aquarius::basic_server<channel_server_session>;
	} // namespace serviced
} // namespace aquarius
