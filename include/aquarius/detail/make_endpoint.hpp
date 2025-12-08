#pragma once
#include <aquarius/asio.hpp>

namespace aquarius
{
	namespace detail
	{
		using endpoint = ip::tcp::endpoint;

		inline endpoint make_v4_endpoint(uint16_t port)
		{
			return endpoint(ip::tcp::v4(), port);
		}

		inline endpoint make_v6_endpoint(uint16_t port)
		{
			return endpoint(ip::tcp::v6(), port);
		}
	} // namespace detail
} // namespace aquarius