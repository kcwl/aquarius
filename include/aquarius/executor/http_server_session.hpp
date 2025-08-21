#pragma once
#include <aquarius/protocol/http.hpp>
#include <aquarius/basic_session.hpp>

namespace aquarius
{
	using http_server_session = basic_session<true, http>;
}