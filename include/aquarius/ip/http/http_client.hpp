#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/http/http_client_session.hpp>

namespace aquarius
{
	using http_client = basic_client<http_client_session>;
} // namespace aquar