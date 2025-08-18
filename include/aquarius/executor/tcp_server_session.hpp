#pragma once
#include <aquarius/basic_tcp_session.hpp>
#include <virgo.hpp>

namespace aquarius
{
	using tcp_server_session = basic_tcp_session<true, virgo::tcp::detail::layer_header>;
}