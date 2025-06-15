#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/ip/tcp.hpp>

namespace aquarius
{
	using tcp_client = basic_client<tcp>;
} // namespace aquarius