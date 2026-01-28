#pragma once
#include <aquarius/ip/default_selector.hpp>
#include <aquarius/ip/raw_socket_adaptor.hpp>
#include <aquarius/ip/session.hpp>

namespace aquarius
{
	template <auto Tag>
	using server_session = session<Tag, raw_socket_adaptor, ip::tcp_selector>;

	template <auto Tag, typename Selector>
	using custom_server_session = session<Tag, raw_socket_adaptor, Selector>;
} // namespace aquarius