#pragma once
#include <aquarius/sql/concepts.hpp>
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/struct_name.hpp>

namespace aquarius
{
	namespace sql
	{
		template <typename T>
		struct remove_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto struct_name = aquarius::detail::struct_name<T>();

				constexpr static auto delete_sql = "delete from "sv;

				return concat_v<delete_sql, struct_name>;
			}
		};

		template <typename T>
		struct enable_condition_view<remove_view<T>> : std::true_type
		{};

		template <typename T>
		struct _remove
		{
			constexpr auto operator()() const
			{
				return remove_view<T>();
			}
		};

		template <typename T>
		inline constexpr _remove<T> remove;
	} // namespace sql
} // namespace aquarius