#pragma once
#include <aquarius/core/impl/time_traits.hpp>

namespace aquarius
{
	using deadline_timer = asio::basic_deadline_timer<std::chrono::system_clock::time_point,
													  impl::time_traits<std::chrono::system_clock::time_point>>;

	using steady_timer = asio::steady_timer;
} // namespace aquarius