#pragma once
#include <aquarius/ip/session.hpp>
#include <aquarius/ip/ssl_socket_adaptor.hpp>

namespace aquarius
{
	template <auto Tag>
	using ssl_server_session = session<Tag, ssl_socket_adaptor<true, Tag>>;
} // namespace aquarius