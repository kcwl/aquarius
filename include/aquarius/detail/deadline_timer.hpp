#pragma once
#include "time_traits.hpp"
#include <boost/asio/deadline_timer.hpp>

namespace aquarius
{
	namespace detail
	{
		using deadline_timer =  boost::asio::basic_deadline_timer<std::chrono::system_clock::time_point,time_traits<std::chrono::system_clock::time_point>>;
	}
}