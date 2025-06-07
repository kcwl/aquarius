#pragma once
#include <aquarius/basic_server.hpp>
#include <aquarius/tcp/session.hpp>
#include <boost/asio/any_io_executor.hpp>

namespace aquarius
{
	namespace tcp
	{
		using async_server = basic_server<server_session>;
	}
} // namespace aquarius