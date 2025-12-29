#pragma once
#include <memory>
#include <set>

namespace aquarius
{
	namespace serviced
	{
		template <typename Session, typename Policy, typename Less>
		class basic_channel
		{
			using role = channel_role<Session>;

		public:
			basic_channel() = default;

		public:
			template <typename Task>
			void publish(Task&& task)
			{
				policy_.publish(subscribers_, std::forward<Task>(task));
			}

			void subscribe(std::shared_ptr<role> subscriber)
			{
				subscribers_.insert(subscriber);
			}

		private:
			std::set<std::shared_ptr<role>, Less> subscribers_;

			Policy policy_;
		};
	} // namespace serviced
} // namespace aquarius