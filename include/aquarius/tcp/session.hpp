#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/package_processor.hpp>
#include <aquarius/tcp/session_service.hpp>
#include <aquarius/ssl/ssl.hpp>

namespace aquarius
{
	namespace tcp
	{
		using session = basic_session<tcp::session_service<>, package_processor>;
	}
} // namespace aquarius