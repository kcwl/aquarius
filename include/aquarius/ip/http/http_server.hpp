#pragma once
#include <aquarius/ip/http/http_server_session.hpp>
#include <aquarius/basic_server.hpp>

namespace aquarius
{
	using http_server = basic_server<http_server_session>;
}