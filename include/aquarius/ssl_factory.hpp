#pragma once
#include <aquarius/config.hpp>
#include <cstring>

namespace aquarius
{
	template <bool Server, auto SSLVersion>
	struct ssl_context_factory;
}