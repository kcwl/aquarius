#pragma once
#include "player.h"
#include <map>
#include <memory>

namespace aquarius
{
	namespace serviced
	{
		template <typename Policy>
		class basic_channel
		{
		public:
			basic_channel() = default;

		public:
			template <typename Task>
			void publish(Task&& task)
			{
				policy_.publish(subscribers_, std::forward<Task>(task));
			}

			void subscribe(std::shared_ptr<player> subscriber)
			{
				subscribers_.emplace(subscriber->id(), subscriber);
			}

		private:
			std::map<std::size_t, std::shared_ptr<player>> subscribers_;

			Policy policy_;
		};
	} // namespace serviced
} // namespace aquarius