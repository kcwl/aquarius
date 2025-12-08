#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/sql/closure.hpp>
#include <aquarius/sql/concepts.hpp>

namespace aquarius
{
	namespace sql
	{
		template <typename T, typename Cond>
		requires(enable_condition<T> || enable_where<T>)
		struct and_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto and_sql = " and "sv;

				constexpr static auto sql = T()();

				using cond_t = std::decay_t<Cond>;

				return concat_v<sql, and_sql, cond_t::left, cond_t::symbol, cond_t::right>;
			}
		};

		template <typename T, typename Cond>
		struct enable_condition_view<and_view<T, Cond>> : std::true_type
		{};

		struct _and : sql_adaptor<_and>
		{
			template <typename T, typename Cond>
			constexpr auto operator()(T&&, Cond&&) const
			{
				return and_view<T, Cond>();
			}

			using sql_adaptor<_and>::operator();
		};

		inline constexpr _and and_;
	} // namespace sql
} // namespace aquarius