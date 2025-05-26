#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/ssl/ssl.hpp>
#include <aquarius/tcp/session_service.hpp>

namespace aquarius
{
	namespace tcp
	{
		using session = basic_session<tcp::session_service<>, package_processor<protocol::tcp>>;
	}
} // namespace aquarius