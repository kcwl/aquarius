#pragma once
#include <aquarius/config.hpp>
#include <boost/asio/error.hpp>

namespace aquarius
{
	namespace error
	{
		enum package
		{
			ok,			// success complete
			pending,	// waiting for handle
			incomplete, // package is not complete
			unknown,	// unknown protocol
			nosession,	// non session
			timeout		// timeout
		};
	}
} // namespace aquarius