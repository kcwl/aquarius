#pragma once
#include "channel.hpp"
#include "player.h"
#include <aquarius/module/module.hpp>

namespace aquarius
{
	namespace serviced
	{
		struct channel_config
		{
			bool enabled;
			int64_t timeout;
		};

		class channel_module : public _module<channel, channel_config>
		{
		public:
			virtual bool enable() override;

			void subscribe(std::string_view name, std::shared_ptr<player> subscribe);

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

			std::map<std::string_view, std::shared_ptr<channel>> channels_;
		};
	} // namespace serviced
} // namespace aquarius