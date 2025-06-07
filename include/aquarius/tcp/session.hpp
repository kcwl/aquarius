#pragma once
#include <aquarius/basic_session.hpp>
#include <aquarius/detail/protocol.hpp>
#include <aquarius/tcp/processor.hpp>

namespace aquarius
{
	namespace tcp
	{
		using client_session = basic_session<false, boost::asio::ip::tcp, processor<boost::asio::ip::tcp>>;

		using server_session = basic_session<true, boost::asio::ip::tcp, processor<boost::asio::ip::tcp>>;
	} // namespace tcp
} // namespace aquarius