#pragma once
#include <aquarius/channel/group.hpp>

namespace aquarius
{
	namespace channel
	{
		class publisher
		{
		public:
			template <typename Channel, typename... Args>
			void call(typename Channel::topic tpc, Args&&... args)
			{
				create_channel_group<Channel>().publish(tpc, std::forward<Args>(args)...);
			}
		};

	} // namespace channel
} // namespace aquarius