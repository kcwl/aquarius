#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/sql/closure.hpp>
#include <aquarius/sql/concepts.hpp>

namespace aquarius
{
	namespace sql
	{
		template <typename T, typename Cond>
		requires enable_select<T>
		struct where_view
		{
			constexpr auto operator()() const -> std::string_view
			{
				constexpr static auto where_sql = " where "sv;

				constexpr static auto spcae = " "sv;

				constexpr static auto sql = T()();

				using cond_t = std::decay_t<Cond>;

				return concat_v<sql, where_sql, cond_t::left, spcae, cond_t::symbol, spcae, cond_t::right>;
			}
		};

		template <typename T, typename Cond>
		struct enable_where_view<where_view<T, Cond>> : std::true_type
		{};

		namespace detail
		{
			template <typename T, typename... Args>
			concept can_where_adaptor = requires { typename where_view<T, Args...>; };
		} // namespace detail

		struct _where : sql_adaptor<_where>
		{
			template <typename T, typename Cond>
			requires detail::can_where_adaptor<T, Cond>
			constexpr auto operator()(T&&, Cond&&) const
			{
				return where_view<T, Cond>();
			}

			using sql_adaptor<_where>::operator();
		};

		inline constexpr _where where;
	} // namespace sql
} // namespace aquarius