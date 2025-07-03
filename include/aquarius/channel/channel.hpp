#pragma once
#include <aquarius/channel/detail/basic_channel.hpp>
#include <aquarius/channel/subscriber.hpp>

namespace aquarius
{
	namespace channel
	{
		using default_channel = detail::basic_channel<subscriber>;
	}
} // namespace aquarius