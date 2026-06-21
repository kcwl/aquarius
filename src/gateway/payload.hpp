#pragma once
#include <random>
#include <set>
#include <aquarius/detail/uuid_generator.hpp>

namespace aquarius
{
	namespace gateway
	{
		template<typename T>
		struct round_robin
		{
			round_robin()
				: index(0)
			{}

			std::shared_ptr<T> invoke(const std::vector<std::shared_ptr<T>>& entities)
			{
				if (entities.empty())
				{
					return nullptr;
				}

				if (index >= entities.size())
				{
					index = 0;
				}

				return entities[index++];
			}

			std::size_t index;
		};

		template<typename T>
		struct weight_first
		{
			constexpr static std::size_t max_weigth = 100;

			std::shared_ptr<T> invoke(const std::vector<std::shared_ptr<T>>& entities)
			{
				if (entities.empty())
				{
					return nullptr;
				}

				std::random_device rd{};
				std::mt19937 gen{ rd() };

				std::uniform_int_distribution<> distrib(0, max_weigth);

				auto weight = distrib(gen);

				for (auto& subscriber : entities)
				{
					if (weight < subscriber->weight())
					{
						return subscriber;
					}
				}

				return nullptr;
			}
		};

		template <typename T>
		struct uuid_hash
		{
			std::shared_ptr<T> invoke(const std::vector<std::shared_ptr<T>>& entities)
			{
				if (entities.empty())
				{
					return nullptr;
				}

				auto index = detail::uuid_generator()() % entities.size();

				return entities[index];
			}
		};

		template <typename Invoke, typename T>
		struct some_min
		{
			std::shared_ptr<T> invoke(const std::vector<std::shared_ptr<T>>& entities)
			{
				if (entities.empty())
				{
					return nullptr;
				}

				int index = 0;
				int target = index;
				std::size_t max_total = std::numeric_limits<std::size_t>::max();
				for (auto& subscriber : entities)
				{
					auto min = Invoke(subscriber)();

					if (min < max_total)
					{
						max_total = min;
						target = index;
					}

					index++;
				}

				if (target >= entities.size())
					return nullptr;

				return entities[target];
			}
		};
	} // namespace serviced
} // namespace aquarius