#pragma once
#include <limits>

namespace aquarius
{
	namespace detail
	{
		template <typename T>
		requires(std::is_floating_point_v<T>)
		bool check_equal(T lhs, T rhs)
		{
			return std::abs(lhs - rhs) < std::numeric_limits<T>::epsilon();
		}
	} // namespace detail
} // namespace aquarius