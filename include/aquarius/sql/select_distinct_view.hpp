#pragma once
#include <aquarius/sql/concepts.hpp>
#include <aquarius/detail/concat.hpp>

namespace aquarius
{
	namespace sql
	{

		template <typename T>
		class select_distinct_view
		{
		public:
			constexpr select_distinct_view() = default;

		public:
			constexpr auto operator()() const
			{
				constexpr static auto struct_name = aquarius::detail::struct_name<T>();

				constexpr static std::string_view sql = "select distinct * from "sv;

				return concat_v<sql, struct_name>;
			}

		private:
			std::string sql_;
		};

		template <typename T>
		struct enable_select_view<select_distinct_view<T>> : std::true_type
		{};

		struct _select_distinct : detail::sql_adaptor_closure
		{
			template <typename T>
			constexpr auto operator()() const
			{
				return select_distinct_view<T>();
			}
		};

		inline constexpr _select_distinct distinct;
	} // namespace sql
} // namespace aquarius