#pragma once
#include <aquarius/ip/http/http_proc.hpp>
#include <aquarius/basic_session.hpp>

namespace aquarius
{
	using http_server_session = basic_session<true, http_proc>;
}