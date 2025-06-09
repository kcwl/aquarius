#pragma once
#include <boost/asio/ip/address.hpp>

namespace aquarius
{
	namespace ip
	{
		using boost::asio::ip::make_address;

		using boost::asio::ip::make_address_v4;

		using boost::asio::ip::make_address_v6;
	} // namespace ip
} // namespace aquarius