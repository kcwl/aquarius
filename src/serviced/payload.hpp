#pragma once
#include "error.hpp"
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

			std::shared_ptr<customer_base> invoke(const std::vector<std::shared_ptr<customer_base>>& customers)
			{
				if (customers.empty())
				{
					return nullptr;
				}

				if (index >= customers.size())
				{
					index = 0;
				}

				return customers[index++];
			}

			std::size_t index;
		};

		struct weight_first
		{
			constexpr static std::size_t max_weigth = 100;

			std::shared_ptr<customer_base> invoke(const std::vector<std::shared_ptr<customer_base>>& customers)
			{
				if (customers.empty())
				{
					return nullptr;
				}

				std::random_device rd{};
				std::mt19937 gen{ rd() };

				std::uniform_int_distribution<> distrib(0, max_weigth);

				auto weight = distrib(gen);

				for (auto& subscriber : customers)
				{
					if (weight < subscriber->weight())
					{
						return subscriber;
					}
				}

				return nullptr;
			}
		};

		template <typename UUID>
		struct uuid_hash
		{
			std::shared_ptr<customer_base> invoke(const std::vector<std::shared_ptr<customer_base>>& customers)
			{
				if (customers.empty())
				{
					return nullptr;
				}

				auto index = UUID()() % customers.size();

				return customers[index];
			}
		};

		template <typename Invoke>
		struct some_min
		{
			std::shared_ptr<customer_base> invoke(const std::vector<std::shared_ptr<customer_base>>& customers)
			{
				if (customers.empty())
				{
					return nullptr;
				}

				int index = 0;
				int target = index;
				std::size_t max_total = std::numeric_limits<std::size_t>::max();
				for (auto& subscriber : customers)
				{
					auto min = Invoke(subscriber)();

					if (min < max_total)
					{
						max_total = min;
						target = index;
					}

					index++;
				}

				if (target >= customers.size())
					return nullptr;

				return customers[target];
			}
		};
	} // namespace serviced
} // namespace aquarius