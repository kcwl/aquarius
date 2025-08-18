#pragma once
#include <aquarius/basic_tcp_session.hpp>
#include <virgo.hpp>

namespace aquarius
{
	using tcp_client_session = basic_tcp_session<false, virgo::tcp::detail::layer_header>;
}