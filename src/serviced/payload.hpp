#pragma once
#include <random>
#include <set>

namespace aquarius
{
	namespace serviced
	{

		struct round_robin
		{
			round_robin()
				: index(0)
			{}

			template <typename Subscriber, typename CompleteToken>
			auto publish(std::set<Subscriber> subscribers, CompleteToken&& token)
			{
				if (subscribers.empty())
					return;

				if (index >= subscribers.size())
				{
					index = 0;
				}

				auto& subscriber = subscribers[index++];

				return subscriber->feedback(token);
			}

			std::size_t index;
		};

		struct weight_first
		{
			constexpr static std::size_t max_weigth = 100;

			template <typename Subscriber, typename CompleteToken>
			auto publish(std::set<Subscriber> subscribers, CompleteToken&& token)
			{
				if (subscribers.empty())
					return;

				std::random_device rd{};
				std::mt19937 gen{ rd() };

				std::uniform_int_distribution<> distrib(0, max_weigth);

				auto weight = distrib(gen);

				for (auto& subscriber : subscribers)
				{
					if (weight < subscriber.weight())
					{
						subscriber->feedback(token);
						break;
					}
				}
			}
		};

		template <typename UUID>
		struct uuid_hash
		{
			template <typename Subscriber, typename Task>
			auto publish(std::set<Subscriber> subscribers, Task&& task)
			{
				if (subscribers.empty())
					return;

				auto index = UUID()() % subscribers.size();

				auto& subscriber = subscribers[index];

				subscriber->feedback(task);
			}
		};

		template <typename Invoke>
		struct some_min
		{
			template <typename Subscriber, typename CompleteToken>
			auto publish(std::set<Subscriber> subscribers, CompleteToken&& token)
			{
				if (subscribers.empty())
					return;

				int index = 0;
				int target = index;
				std::size_t max_total = std::numeric_limits<std::size_t>::max();
				for (auto& subscriber : subscribers)
				{
					auto min = Invoke(subscriber)();

					if (min < max_total)
					{
						max_total = min;
						target = index;
					}

					index++;
				}

				if (target >= subscribers.size())
					return;

				auto& subscriber = subscribers[target];

				subscriber->feedback(token);
			}
		};
	} // namespace serviced
} // namespace aquarius