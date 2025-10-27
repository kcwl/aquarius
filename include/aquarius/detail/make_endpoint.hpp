#pragma once
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	namespace detail
	{
		using endpoint = boost::asio::ip::tcp::endpoint;

		inline endpoint make_v4_endpoint(uint16_t port)
		{
			return endpoint(boost::asio::ip::tcp::v4(), port);
		}

		inline endpoint make_v6_endpoint(uint16_t port)
		{
			return endpoint(boost::asio::ip::tcp::v6(), port);
		}
	} // namespace detail
} // namespace aquarius