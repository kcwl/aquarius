#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/tcp/session.hpp>

namespace aquarius
{
	namespace tcp
	{
		using async_client = basic_client<boost::asio::ip::tcp, tcp::session>;
	}
} // namespace aquarius