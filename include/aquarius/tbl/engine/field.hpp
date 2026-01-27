#pragma once
#include <tuple>

namespace aquarius
{
	namespace tbl
	{
		template<typename... Args>
		struct field
		{
			using type = std::tuple<Args...>;
		};
	} // namespace tbl
} // namespace aquarius