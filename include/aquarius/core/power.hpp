#pragma once
#include <cstring>

namespace aquarius
{
	template <int Back, int Exponent>
	struct power
	{
		constexpr static std::size_t value = power<Back, Exponent - 1>::value * Back;
	};

	template <int Back>
	struct power<Back, 0>
	{
		constexpr static std::size_t value = 1;
	};
}