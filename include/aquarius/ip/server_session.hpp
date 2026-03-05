#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/ip/adaptor/raw_socket_adaptor.hpp>

namespace aquarius
{
	template <typename Protocol, typename Selector>
	using server_session = basic_session<Protocol, raw_socket_adaptor, Selector>;
} // namespace aquarius