#pragma once
#include <aquarius/logger.hpp>
#include <map>
#include <mutex>
#include <string_view>

namespace aquarius
{
	namespace serviced
	{
		template <typename Channel>
		class channel_group
		{
		public:
			template <typename Subscriber>
			void subscribe(std::string_view name, std::shared_ptr<Subscriber> subscribe)
			{
				std::lock_guard lock(mutex_);

				auto& chan = channels_[name];

				if (!chan)
					chan = std::make_shared<Channel>();

				chan->subscribe(subscribe);
			}

			template <typename Task>
			auto publish(std::string_view name, Task&& task)
			{
				std::lock_guard lock(mutex_);

				auto it = channels_.find(name);

				if (it == channels_.end())
				{
					XLOG_ERROR() << "channel [" << name << "] not found";

					return;
				}

				it->second->publish(std::forward<Task>(task));
			}

		private:
			std::mutex mutex_;

			std::map<std::string_view, std::shared_ptr<Channel>> channels_;
		};
	} // namespace serviced
} // namespace aquarius