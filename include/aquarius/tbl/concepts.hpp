#pragma once
#include <concepts>

namespace aquarius
{
	namespace tbl
	{
		template <typename T>
		struct enable_condition_view : std::false_type
		{};

		template <typename T>
		struct enable_select_view : std::false_type
		{};

		template <typename T>
		struct enable_where_view : std::false_type
		{};

		template <typename T>
		concept enable_select = enable_select_view<std::remove_cvref_t<T>>::value;

		template <typename T>
		concept enable_condition = enable_condition_view<std::remove_cvref_t<T>>::value;

		template <typename T>
		concept enable_where = enable_where_view<std::remove_cvref_t<T>>::value;
	} // namespace sql
} // namespace aquarius