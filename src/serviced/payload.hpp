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

			auto publish(std::map<std::size_t, std::shared_ptr<player>>& subscribers, flex_buffer& buffer)
				-> awaitable<bool>
			{
				if (subscribers.empty())
					co_return false;

				if (index >= subscribers.size())
				{
					index = 0;
				}

				auto& subscriber = subscribers[index++];

				co_return co_await subscriber->feedback(buffer);
			}

			std::size_t index;
		};

		struct weight_first
		{
			constexpr static std::size_t max_weigth = 100;

			auto publish(std::map<std::size_t, std::shared_ptr<player>>& subscribers, flex_buffer& buffer)
				-> awaitable<bool>
			{
				if (subscribers.empty())
					co_return false;

				std::random_device rd{};
				std::mt19937 gen{ rd() };

				std::uniform_int_distribution<> distrib(0, max_weigth);

				auto weight = distrib(gen);

				for (auto& subscriber : subscribers)
				{
					if (weight < subscriber.second->weight())
					{
						co_return co_await subscriber.second->feedback(buffer);
					}
				}

				co_return false;
			}
		};

		template <typename UUID>
		struct uuid_hash
		{
			auto publish(std::map<std::size_t, std::shared_ptr<player>>& subscribers, flex_buffer& buffer)
				-> awaitable<bool>
			{
				if (subscribers.empty())
					co_return false;

				auto index = UUID()() % subscribers.size();

				auto& subscriber = subscribers[index];

				co_return co_await subscriber->feedback(buffer);
			}
		};

		template <typename Invoke>
		struct some_min
		{
			auto publish(std::map<std::size_t, std::shared_ptr<player>>& subscribers, flex_buffer& buffer)
				-> awaitable<bool>
			{
				if (subscribers.empty())
					co_return false;

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
					co_return false;

				auto& subscriber = subscribers[target];

				co_return co_await subscriber->feedback(buffer);
			}
		};
	} // namespace serviced
} // namespace aquarius