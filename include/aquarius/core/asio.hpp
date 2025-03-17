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

	inline uint32_t ip_addr_to_uint(const std::string& ip_addr)
	{
		return boost::asio::ip::address_v4().from_string(ip_addr).to_uint();
	}

	inline std::string ip_addr_to_string(uint32_t ip_addr)
	{
		return boost::asio::ip::address_v4(ip_addr).to_string();
	}
}