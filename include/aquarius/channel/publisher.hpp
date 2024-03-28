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
			template <typename _Func, typename... _Args>
			void publish(const std::string& topic, _Func&& f, _Args&&... args)
			{
				default_group::instance().publish(topic, std::forward<_Func>(f), std::forward<_Args>(args)...);
			}
		};
	} // namespace channel

} // namespace aquarius