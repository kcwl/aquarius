#pragma once
#include <boost/asio/connect.hpp>

namespace aquarius
{
	namespace detail
	{
		using boost::asio::async_connect;
	}
} // namespace aquarius