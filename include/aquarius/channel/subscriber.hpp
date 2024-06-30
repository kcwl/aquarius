#pragma once
#include <aquarius/channel/watcher.hpp>
#include <memory>

namespace aquarius
{
	template <typename _Topic, typename _Func>
	class subscriber : public std::enable_shared_from_this<subscriber<_Topic, _Func>>, public channel_role<_Topic, _Func>
	{
	public:
		subscriber() = default;

		virtual ~subscriber()
		{
			watcher<_Topic, _Func>::instance().unsubscribe(topic_, this->uuid());
		}

	public:
		virtual bool accept(const _Topic& topic, _Func&& f) override
		{
			topic_ = topic;

			watcher<_Topic, _Func>::instance().subscribe(topic, this->uuid(), std::forward<_Func>(f));
		}

	private:
		_Topic topic_;
	};
} // namespace aquarius