#pragma once
#include <aquarius/ip/raw_socket_adaptor.hpp>
#include <aquarius/ip/session.hpp>

namespace aquarius
{
	template <auto Tag>
	using server_session = session<Tag, raw_socket_adaptor<Tag>>;
} // namespace aquarius