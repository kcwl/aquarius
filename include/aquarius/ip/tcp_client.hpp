#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/tcp.hpp>

namespace aquarius
{
	namespace ip
	{
		using tcp_client = basic_client<tcp>;
	}
} // namespace aquarius