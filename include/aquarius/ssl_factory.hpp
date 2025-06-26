#pragma once
#include <cstring>

namespace aquarius
{
	template <bool Server, auto SSLVersion>
	struct ssl_context_factory;
}