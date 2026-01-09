#pragma once
#include <aquarius/asio.hpp>
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

			template <typename CompleteToken>
			auto publish(std::map<std::size_t, std::shared_ptr<player>>& subscribers, CompleteToken&& token) -> awaitable<void>
			{
				if (subscribers.empty())
					co_return;

				if (index >= subscribers.size())
				{
					index = 0;
				}

				auto& subscriber = subscribers[index++];

				co_await subscriber->feedback(token);
			}

			std::size_t index;
		};

		struct weight_first
		{
			constexpr static std::size_t max_weigth = 100;

			template <typename CompleteToken>
			auto publish(std::map<std::size_t, std::shared_ptr<player>>& subscribers, CompleteToken&& token)
				-> awaitable<void>
			{
				if (subscribers.empty())
					co_return;

				std::random_device rd{};
				std::mt19937 gen{ rd() };

				std::uniform_int_distribution<> distrib(0, max_weigth);

				auto weight = distrib(gen);

				for (auto& subscriber : subscribers)
				{
					if (weight < subscriber.weight())
					{
						co_await subscriber->feedback(token);
						break;
					}
				}
			}
		};

		template <typename UUID>
		struct uuid_hash
		{
			template <typename Task>
			auto publish(std::map<std::size_t, std::shared_ptr<player>>& subscribers, Task&& task)
				-> awaitable<void>
			{
				if (subscribers.empty())
					co_return;

				auto index = UUID()() % subscribers.size();

				auto& subscriber = subscribers[index];

				co_await subscriber->feedback(task);
			}
		};

		template <typename Invoke>
		struct some_min
		{
			template <typename CompleteToken>
			auto publish(std::map<std::size_t, std::shared_ptr<player>>& subscribers, CompleteToken&& token)
				-> awaitable<void>
			{
				if (subscribers.empty())
					co_return;

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
					co_return;

				auto& subscriber = subscribers[target];

				co_await subscriber->feedback(token);
			}
		};
	} // namespace serviced
} // namespace aquarius