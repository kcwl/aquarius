#pragma once
#include <aquarius/sql/concepts.hpp>
#include <aquarius/detail/concat.hpp>

namespace aquarius
{
	namespace sql
	{

		template <typename T, typename Cond>
		requires enable_select<T>
		struct limit_view
		{
			constexpr auto operator()() const
			{
				constexpr static auto sql = T()();

				constexpr static auto limit_sql = " limit "sv;

				using cond_t = std::decay_t<Cond>;

				constexpr static auto cond_limit = cond_t()()();

				return concat_v<sql, limit_sql, cond_limit>;
			}
		};

		struct _limit : sql_adaptor<_limit>
		{
			template <typename T, typename Cond>
			requires enable_select<T>
			constexpr auto operator()(T&&, Cond&&) const
			{
				return limit_view<T, Cond>();
			}

			using sql_adaptor<_limit>::operator();
		};

		inline constexpr _limit limit;
	} // namespace sql
} // namespace aquarius