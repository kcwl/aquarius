#pragma once
#include <aquarius/core/core.hpp>
#include <functional>

namespace aquarius
{
	template <typename _Watcher, typename _Type = int>
	class subscriber : public role
	{
	public:
		using func_t = typename _Watcher::func_t;

		using topic_t = typename _Watcher::topic_t;

	public:
		subscriber() = default;

		virtual ~subscriber()
		{
			_Watcher::instance().unsubscribe(topic_, this->uuid());
		}

	public:
		bool subscribe(const topic_t& topic, func_t&& f)
		{
			topic_ = topic;

			_Watcher::instance().subscribe(topic, this->uuid(), std::forward<func_t>(f));

			return true;
		}

	private:
		topic_t topic_;
	};

	template <typename _Watcher>
	class subscriber<_Watcher, subscriber_global> : public role
	{
		using func_t = typename _Watcher::func_t;

		using topic_t = typename _Watcher::topic_t;

	public:
		bool subscribe(const topic_t& topic, func_t&& f)
		{
			_Watcher::instance().subscribe(topic, this->uuid(), std::forward<func_t>(f));

			return true;
		}
	};
} // namespace aquarius