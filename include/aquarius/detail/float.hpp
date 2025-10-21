#pragma once
#include <concepts>
#include <cmath>
#include <limits>

namespace aquarius
{
	template <typename T>
	requires(std::is_floating_point_v<T>)
	bool check_equal(T lhs, T rhs)
	{
		return std::abs(lhs - rhs) < std::numeric_limits<T>::epsilon();
	}
} // namespace aquarius