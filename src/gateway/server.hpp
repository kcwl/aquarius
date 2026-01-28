#pragma once
#include "default_selector.hpp"
#include "transfer_handler.hpp"
#include <aquarius/basic_server.hpp>
#include <aquarius/ip/handler.hpp>
#include <aquarius/ip/server_session.hpp>

namespace aquarius
{
	namespace gateway
	{
		using transfer_server_session = custom_server_session<proto::http, transfer_selector>;

		using server = aquarius::basic_server<transfer_server_session>;
	} // namespace gateway
} // namespace aquarius

#define AQUARIUS_GATEWAY_HANDLER(__request, __response, __method)                                                      \
	AQUARIUS_CONTEXT_BY(aquarius::gateway::transfer_server_session, __request, __response, __method)