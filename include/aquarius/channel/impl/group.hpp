#pragma once
#include <aquarius/core/singleton.hpp>
#include <mutex>
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

				using subscriber_t = typename channel_t::role_t;

			public:
				std::shared_ptr<channel_t> find(const std::string& topic)
				{
					std::shared_lock lk(mutex_);

					auto iter = channels_.find(topic);

					if (iter == channels_.end())
						return nullptr;

					return iter->second;
				}

				void erase(const std::string& topic)
				{
					std::unique_lock lk(mutex_);

					channels_.erase(topic);
				}

				void subscribe(const std::string& topic, subscriber_t* subscriber_role)
				{
					std::unique_lock lk(mutex_);

					auto iter = channels_.find(topic);

					if (iter != channels_.end())
					{
						iter->second->subscribe(subscriber_role);

						return;
					}

					auto channel = std::make_shared<channel_t>();

					channel->subscribe(subscriber_role);

					channels_[topic] = channel;
				}

				template<typename... _Args>
				bool publish(const std::string& topic, const std::string& command, _Args&&... args)
				{
					std::unique_lock lk(mutex_);

					auto iter = channels_.find(topic);

					if (iter == channels_.end())
						return false;

					iter->second->call(command, std::forward<_Args>(args)...);

					return true;
				}

			private:
				std::shared_mutex mutex_;

				std::unordered_map<std::string, std::shared_ptr<channel_t>> channels_;
			};
		} // namespace impl
	}	  // namespace channel
} // namespace aquarius