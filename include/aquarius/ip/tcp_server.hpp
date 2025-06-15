#pragma once
#include <aquarius/basic_server.hpp>
#include <boost/asio/any_io_executor.hpp>

namespace aquarius
{
	using tcp_server = basic_server<tcp>;
} // namespace aquarius