#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/package_processor.hpp>
#include <aquarius/ssl/no_ssl_protocol.hpp>
#include <aquarius/tcp/async_io_service.hpp>

namespace aquarius
{
	namespace tcp
	{
		using async_session = basic_session<tcp::async_io_service<ssl::no_ssl_protocol>, package_processor>;
	}
} // namespace aquarius