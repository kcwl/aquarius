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

			template<typename U>
			field& operator=(const U& u)
			{
				value = u;

				return *this;
			}

			value_type value;
		};
	} // namespace tbl
} // namespace aquarius