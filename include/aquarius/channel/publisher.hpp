#pragma once
#include <aquarius/channel/group.hpp>
#include <aquarius/channel/impl/role.hpp>
#include <string>

namespace aquarius
{
	namespace channel
	{
		class publisher : public impl::role
		{
		public:
			auto publish(const std::string& topic, const std::string& command)
			{
				return default_group::instance().publish(topic, command);
			}
		};
	} // namespace channel

} // namespace aquarius