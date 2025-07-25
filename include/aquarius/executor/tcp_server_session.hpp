#pragma once
#include <aquarius/basic_tcp_session.hpp>
#include <aquarius/flow/stream_flow.hpp>
#include <aquarius_protocol.hpp>

namespace aquarius
{
	using tcp_server_session = basic_tcp_session<true, stream_flow, basic_tcp_header<true>>;
}