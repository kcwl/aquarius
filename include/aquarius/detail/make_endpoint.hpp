#pragma once
#include <aquarius/detail/asio.hpp>

namespace aquarius
{
	namespace detail
	{
        inline typename asio::ip::tcp::endpoint make_endpoint(uint16_t port)
        {
            using endpoint = asio::ip::tcp::endpoint;
#ifdef USE_IPV6
            return endpoint(asio::ip::tcp::v6(), port);
#else
            return endpoint(asio::ip::tcp::v4(), port);
#endif
        }
	} // namespace detail
} // namespace aquarius