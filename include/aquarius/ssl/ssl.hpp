#pragma once
#ifdef AQUARIUS_ENABLE_SSL
#include <boost/asio/ssl.hpp>

namespace aquarius
{
	namespace ssl
	{
		using context = boost::asio::ssl::context;
	}
}
#endif