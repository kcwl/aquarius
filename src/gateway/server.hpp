#pragma once
#include "default_selector.hpp"
#include "transfer_handler.hpp"
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	namespace gateway
	{
		using transfer_server_session = server_session<transfer_http_selector>;

		using server = aquarius::basic_server<transfer_server_session>;
	} // namespace gateway
} // namespace aquarius
