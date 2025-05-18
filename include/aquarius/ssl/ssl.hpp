#pragma once
#include <boost/asio/ssl.hpp>

#ifdef AQUARIUS_ENABLE_SSL
namespace aquarius
{
	namespace ssl
	{
		using context = boost::asio::ssl::context;
	}
}
#endif