#pragma once
#include <boost/asio/deadline_timer.hpp>
#include "time_traits.hpp"


namespace aquarius
{
	namespace core
	{
		using deadline_timer =  boost::asio::basic_deadline_timer<std::chrono::system_clock::time_point,time_traits<std::chrono::system_clock::time_point>>;
	}
}