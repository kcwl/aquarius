#pragma once
#include <aquarius/channel/watcher.hpp>
#include <functional>

namespace aquarius
{
	template <typename _Topic, typename _Func = std::function<void()>>
	class subscriber : public role<_Topic>
	{
	public:
		subscriber() = default;

		virtual ~subscriber()
		{
			watcher<_Topic, _Func>::instance().unsubscribe(topic_, this->uuid());
		}

	public:
		bool subscribe(const _Topic& topic, _Func&& f)
		{
			topic_ = topic;

			watcher<_Topic, _Func>::instance().subscribe(topic, this->uuid(), std::forward<_Func>(f));
		}

	private:
		_Topic topic_;
	};
} // namespace aquarius