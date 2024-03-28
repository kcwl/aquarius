#pragma once
#include <aquarius/channel/channel.hpp>
#include <aquarius/core/singleton.hpp>
#include <shared_mutex>
#include <unordered_map>

namespace aquarius
{
	namespace channel
	{
		namespace impl
		{
			template <typename _Channel>
			class basic_group : public singleton<basic_group<_Channel>>
			{
				using channel_t = _Channel;

			public:
				std::shared_ptr<channel_t> find(const std::string& topic)
				{
					std::shared_lock lk(mutex_);

					auto iter = channels_.find(topic);

					if (iter == channels_.end())
						return nullptr;

					return iter->second;
				}

				void push_back(std::shared_ptr<channel_t> channel)
				{
					std::unique_lock lk(mutex_);

					channels_.emplace(channel->topic, channel);
				}

				void earse(const std::string& topic)
				{
					std::unique_lock lk(mutex_);

					channels_.erase(topic);
				}

				void subscribe(const std::string& topic, std::shared_ptr<subscriber> subscriber_role)
				{
					std::unique_lock lk(mutex_);

					auto iter = channels_.find(topic);

					if (iter == channels_.end())
						return;

					iter->second->subscribe(subscriber_role);
				}

				template<typename _Func, typename... _Args>
				void publish(const std::string& topic, _Func&& f, _Args&&... args)
				{
					std::unique_lock lk(mutex_);

					auto iter = channels_.find(topic);

					if (iter == channels_.end())
						return;

					iter->second->call(std::forward<_Func>(f), std::forward<_Args>(args)...);
				}

			private:
				std::shared_mutex mutex_;

				std::unordered_map<std::string, std::shared_ptr<channel_t>> channels_;
			};
		} // namespace impl
	}	  // namespace channel
} // namespace aquarius