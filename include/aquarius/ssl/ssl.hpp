#pragma once
#ifdef AQUARIUS_ENABLE_SSL
#include <boost/asio/ssl.hpp>

namespace aquarius
{
	namespace session
	{
		namespace ssl
		{
			using boost::asio::ssl::context;
		}
	} // namespace session
} // namespace aquarius
#endif