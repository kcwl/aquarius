#pragma once
#include <aquarius/asio.hpp>

namespace aquarius
{
	namespace detail
	{
        template<typename Protocol>
        inline typename Protocol::endpoint make_endpoint(uint16_t port)
        {
            using endpoint = typename Protocol::endpoint;
#ifdef USE_IPV6
            return endpoint(Protocol::v6(), port);
#else
            return endpoint(Protocol::v4(), port);
#endif
        }
	} // namespace detail
} // namespace aquarius