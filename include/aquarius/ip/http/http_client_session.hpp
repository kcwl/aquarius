#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/http/http_proc.hpp>

namespace aquarius
{
	using http_client_session = basic_session<false, http_proc>;
}