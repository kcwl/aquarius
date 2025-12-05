#pragma once
#include <aquarius/ip/session.hpp>
#include <aquarius/ip/socket_adaptor.hpp>

namespace aquarius
{
	template <auto Tag>
	using client_session = session<Tag, socket_adaptor<false, Tag>>;

	template <auto Tag>
	using ssl_client_session = session<Tag, ssl_socket_adaptor<false, Tag>>;
} // namespace aquarius