#pragma once
#include <aquarius/core/time_traits.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace aquarius
{
	namespace core
	{
		using deadline_timer = boost::asio::basic_deadline_timer<std::chrono::system_clock::time_point,
																 time_traits<std::chrono::system_clock::time_point>>;

		using steady_timer = boost::asio::steady_timer;
	} // namespace detail
} // namespace aquarius