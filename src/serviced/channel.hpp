#pragma once
#include "basic_channel.hpp"
#include "payload.hpp"

namespace aquarius
{
	namespace serviced
	{
		using channel = basic_channel<round_robin>;

	} // namespace serviced
} // namespace aquarius