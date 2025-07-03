#pragma once
#include <aquarius/channel/detail/basic_group.hpp>

namespace aquarius
{
	namespace channel
	{
		template <typename Channel>
		detail::basic_group<Channel> create_channel_group()
		{
			static detail::basic_group<Channel> g;

			return g;
		}
	} // namespace channel
} // namespace aqauruis