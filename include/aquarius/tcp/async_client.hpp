#pragma once
#include <aquarius/basic_client.hpp>
#include <aquarius/tcp/async_session.hpp>

namespace aquarius
{
	namespace tcp
	{
		using async_client = basic_client<boost::asio::ip::tcp, tcp::async_session>;
	}
} // namespace aquarius