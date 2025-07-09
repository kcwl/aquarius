#pragma once
#include <aquarius/config.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/steady_timer.hpp>

namespace aquarius
{
	using boost::asio::deadline_timer;

	using boost::asio::steady_timer;
} // namespace aquarius