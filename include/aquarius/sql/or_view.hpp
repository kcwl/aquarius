#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/sql/concepts.hpp>

namespace aquarius
{
	namespace sql
	{
		template <typename T, typename Cond>
		requires(enable_condition<T> || enable_where<T>)
		struct or_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto and_sql = " or "sv;

				constexpr static auto sql = T()();

				using cond_t = std::decay_t<Cond>;

				return concat_v<sql, and_sql, cond_t::left, cond_t::symbol, cond_t::right>;
			}
		};

		template <typename T, typename Cond>
		struct enable_condition_view<or_view<T, Cond>> : std::true_type
		{};

		struct _or : sql_adaptor<_or>
		{
			template <typename T, typename Cond>
			constexpr auto operator()(T&&, Cond&&) const
			{
				return or_view<T, Cond>();
			}

			using sql_adaptor<_or>::operator();
		};

		inline constexpr _or or_;
	} // namespace sql
} // namespace aquarius