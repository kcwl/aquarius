#pragma once
#include <aquarius/channel/role.hpp>
#include <aquarius/channel/watcher.hpp>
#include <functional>

namespace aquarius
{
	template <typename _Watcher>
	class publisher : public role
	{
	public:
		using topic_t = typename _Watcher::topic_t;
		
	public:
		bool publish(const topic_t& topic)
		{
			return _Watcher::instance().publish(topic);
		}
	};

} // namespace aquarius