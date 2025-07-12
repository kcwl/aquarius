#pragma once
#include <boost/asio/io_context.hpp>

namespace aquarius
{
	namespace io
	{
		class mysql_io : public boost::asio::io_context
		{
		public:
			mysql_io() = default;

		public:

		};
	}
}