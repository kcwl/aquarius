#pragma once
#include <aquarius/channel/role.hpp>
#include <aquarius/channel/watcher.hpp>
#include <functional>

namespace aquarius
{
	template <typename _Topic, typename _Func = std::function<void()>>
	class publisher : public role<_Topic>
	{
	public:
		bool publish(const _Topic& topic)
		{
			return watcher<_Topic, _Func>::instance().publish(topic);
		}
	};

} // namespace aquarius