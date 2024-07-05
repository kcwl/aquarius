#pragma once
#include <aquarius/channel/watcher.hpp>
#include <aquarius/core/core.hpp>
#include <functional>

namespace aquarius
{
	template <typename _Type, typename _Topic, typename _Func>
	class subscriber;

	template <typename _Topic, typename _Func>
	class subscriber<subscriber_global, _Topic, _Func> : public role<_Topic>
	{
	public:
		bool subscribe(const _Topic& topic, _Func&& f)
		{
			watcher<_Topic, _Func>::instance().subscribe(topic, this->uuid(), std::forward<_Func>(f));

			return true;
		}
	};

	template <typename _Topic, typename _Func>
	class subscriber<int, _Topic, _Func> : public role<_Topic>
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

			return true;
		}

	private:
		_Topic topic_;
	};
} // namespace aquarius