#pragma once
#include <aquarius/ip/adaptor/raw_socket_adaptor.hpp>
#include <aquarius/ip/session.hpp>

namespace aquarius
{
	template <typename Protocol, typename Selector>
	using server_session = session<Protocol, raw_socket_adaptor, Selector>;
} // namespace aquarius