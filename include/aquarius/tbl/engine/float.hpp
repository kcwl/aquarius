#pragma once
#include <limits>
#include <string_view>

namespace aquarius
{
	namespace tbl
	{

		template <typename T>
		struct _float
		{
			constexpr static auto name = " float"sv;

			static constexpr auto max_length = std::numeric_limits<float>::max();
		};

		template <typename T>
		struct _double
		{
			constexpr static auto name = " double"sv;

			static constexpr auto max_length = std::numeric_limits<double>::max();
		};
	} // namespace tbl
} // namespace aquarius