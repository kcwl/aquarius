#pragma once
#ifdef AQUARIUS_ENABLE_SSL
#include <cstring>

namespace aquarius
{
	namespace session
	{
		template <bool Server, auto SSLVersion>
		struct ssl_context_factory;
	}
} // namespace aquarius
#endif