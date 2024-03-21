#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace aquarius
{
	namespace asio
	{
		using namespace boost::asio;

		using error_code = boost::system::error_code;

		using socket_t = ip::tcp::socket;

		using ssl_socket_t = ssl::stream<socket_t&>;

		using ssl_context_t = ssl::context;
	}
}