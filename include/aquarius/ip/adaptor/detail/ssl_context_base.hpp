#pragma once
#include <aquarius/asio.hpp>

namespace aquarius
{
	class ssl_context_base
	{
	public:
		virtual ssl::context create() = 0;
	};
} // namespace aquarius