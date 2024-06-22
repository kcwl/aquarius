#pragma once
#include <aquarius/channel/role.hpp>
#include <aquarius/channel/watcher.hpp>
#include <aquarius/channel/visitor.hpp>
#include <functional>

namespace aquarius
{
	template <typename _Topic, typename _Func = std::function<void()>>
	class publisher : public channel_role<_Topic, _Func>, public visitor<_Topic,_Func>
	{
	public:
		virtual bool accept(const _Topic& topic, _Func&&)
		{
			return watcher<_Topic, _Func>::instance().publish(topic);
		}
	};

} // namespace aquarius