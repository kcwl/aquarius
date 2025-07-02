#pragma once
#include <boost/asio/ip/tcp.hpp>

namespace aquarius
{
	namespace ip
	{
		namespace detail
		{
			class tcp_base
			{
			public:
				using impl_type = boost::asio::ip::tcp;

				using socket = typename impl_type::socket;

				using no_delay = typename impl_type::no_delay;

				using acceptor = typename impl_type::acceptor;

				using resolver = typename impl_type::resolver;

				using endpoint_type = typename acceptor::endpoint_type;

			public:
				static endpoint_type make_v4_endpoint(uint16_t port)
				{
					return endpoint_type(impl_type::v4(), port);
				}

				static endpoint_type make_v6_endpoint(uint16_t port)
				{
					return endpoint_type(impl_type::v6(), port);
				}
			};
		} // namespace detail
	} // namespace ip
} // namespace aquarius