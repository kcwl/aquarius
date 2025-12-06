#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/struct_name.hpp>
#include <aquarius/sql/concepts.hpp>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace sql
	{
		template <typename T>
		struct select_view
		{
			constexpr auto operator()() const -> std::string_view
			{
				constexpr static auto struct_name = aquarius::detail::struct_name<T>();

				constexpr static std::string_view sql = "select * from "sv;

				return concat_v<sql, struct_name>;
			}
		};

		template <typename T>
		struct enable_select_view<select_view<T>> : std::true_type
		{};

		template <typename T>
		struct enable_condition_view<select_view<T>> : std::true_type
		{};

		template <typename T>
		struct _select
		{
			constexpr auto operator()() const
			{
				return select_view<T>();
			}
		};

		template <typename T>
		inline constexpr _select<T> select;
	} // namespace sql
} // namespace aquarius