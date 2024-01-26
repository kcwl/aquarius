#pragma once
#include <random>

namespace aquarius
{
	inline std::size_t random(std::size_t begin, std::size_t end)
	{
		std::random_device rd{};
		std::mt19937 gen{ rd() };

		std::vector<std::size_t> temp_random{};

		if (begin >= end)
			return begin;

		temp_random.resize(end - begin + 1);

		for (std::size_t i = begin; i < end; ++i)
		{
			temp_random.push_back(i);
		}

		std::shuffle(temp_random.begin(), temp_random.end(), gen);

		return *temp_random.begin();
	}
}