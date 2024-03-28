#pragma once
#include <aquarius/channel/impl/channel.hpp>
#include <aquarius/channel/subscriber.hpp>

namespace aquarius
{
	namespace channel
	{
		using default_channel = impl::basic_channel<subscriber>;
	} // namespace channel

} // namespace aquarius