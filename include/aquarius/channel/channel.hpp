#pragma once
#include <aquarius/channel/role.hpp>
#include <mutex>
#include <unordered_map>

namespace aquarius
{
	template <typename _Topic, typename _Func>
	class channel
	{
		using topic_t = _Topic;

		using func_t = _Func;

		using role_t = channel_role<topic_t, _Func>;

	public:
		channel() = default;
		~channel() = default;

	public:
		topic_t topic() const
		{
			return topic_;
		}

		void subscribe(std::size_t role_id, func_t&& f)
		{
			std::lock_guard lk(mutex_);

			subscribers_.insert({ role_id, std::forward<func_t>(f) });
		}

		void unsubscribe(std::size_t uid)
		{
			std::lock_guard lk(mutex_);

			subscribers_.erase(uid);
		}

		void call()
		{
			for (auto& subs : subscribers_)
			{
				auto f = subs.second;

				if (!f)
					continue;
				
				f();
			}
		}

	private:
		topic_t topic_;

		std::mutex mutex_;

		std::unordered_map<std::size_t, func_t> subscribers_;
	};
} // namespace aquarius