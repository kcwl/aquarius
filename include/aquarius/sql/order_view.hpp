#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/sql/concepts.hpp>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace sql
	{
		struct order_desc
		{
			constexpr static auto value = " desc"sv;
		};

		struct order_asc
		{
			constexpr static auto value = " asc"sv;
		};

		template <typename T, typename Sort, typename Cond>
		requires enable_select<T>
		struct order_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto sql = T()();

				constexpr static auto order_sql = " order by "sv;

				using cond_t = std::decay_t<Cond>;

				constexpr static auto sort_sql = Sort::value;

				constexpr static auto cond_sql = cond_t()()();

				return concat_v<sql, order_sql, cond_sql, sort_sql>;
			}
		};

		template <typename Sort>
		struct _order : sql_adaptor<_order<Sort>>
		{
			template <typename T, typename Cond>
			requires(enable_select<T>)
			constexpr auto operator()(T&&, Cond&&) const
			{
				return order_view<T, Sort, Cond>();
			}

			using sql_adaptor<_order>::operator();
		};

		inline constexpr _order<order_desc> desc_odr;
		inline constexpr _order<order_asc> asc_odr;
	} // namespace sql
} // namespace aquarius