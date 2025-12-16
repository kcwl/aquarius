#pragma once
#include <tuple>

namespace aquarius
{
	namespace tbl
	{
		template <typename T, typename... Args>
		struct field
		{
			using value_type = T;

			using attr_type = std::tuple<Args...>;
		};
	} // namespace tbl
} // namespace aquarius