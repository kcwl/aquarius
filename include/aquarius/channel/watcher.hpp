#pragma once
#include <aquarius/channel/channel.hpp>
#include <aquarius/core/singleton.hpp>
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace aquarius
{
	template<typename _Topic, typename _Func>
	class watcher : public singleton<watcher<_Topic,_Func>>
	{
	public:
		using topic_t = _Topic;

		using func_t = _Func;


		using channel_t = channel<topic_t, func_t>;
	public:
		std::shared_ptr<channel_t> find(const topic_t& topic)
		{
			std::shared_lock lk(mutex_);

			auto iter = channels_.find(topic);

			if (iter == channels_.end())
				return nullptr;

			return iter->second;
		}

		void erase(const topic_t& topic)
		{
			std::unique_lock lk(mutex_);

			channels_.erase(topic);
		}

		void subscribe(const topic_t& topic, std::size_t role_id, func_t&& f)
		{
			std::unique_lock lk(mutex_);

			auto iter = channels_.find(topic);

			if (iter != channels_.end())
			{
				iter->second->subscribe(role_id, std::forward<_Func>(f));

				return;
			}

			auto channel = std::make_shared<channel_t>();

			channel->subscribe(role_id, std::forward<_Func>(f));

			channels_.emplace(topic,channel);
		}

		void unsubscribe(const topic_t& topic, std::size_t role_id)
		{
			std::unique_lock lk(mutex_);

			auto iter = channels_.find(topic);

			if(iter == channels_.end())
				return;

			channels_.erase(iter);
		}

		bool publish(const topic_t& topic)
		{
			std::unique_lock lk(mutex_);
	
			auto iter = channels_.find(topic);

			if (iter == channels_.end())
				return false;

			iter->second->call();

			return true;
		}

	private:
		std::shared_mutex mutex_;

		std::unordered_map<topic_t, std::shared_ptr<channel_t>> channels_;
	};

} // namespace aquarius