#pragma once
#include <concepts>

namespace aquarius
{
	namespace tbl
	{
		template <typename Adaptor, typename... Args>
		concept adaptor_invocable = requires { std::declval<Adaptor>()(std::declval<Args>()...); };

		struct sql_adaptor_closure
		{
			template <typename Self, typename T>
			requires std::derived_from<std::remove_cvref_t<Self>, sql_adaptor_closure> && adaptor_invocable<Self, T>
			friend constexpr auto operator|(T&& t, Self&& self)
			{
				return std::forward<Self>(self)(std::forward<T>(t));
			}
		};

		template <typename Adaptor, typename T, typename... Args>
		concept partical_adaptor_invocable =
			requires { std::declval<Adaptor>()(std::declval<T>(), std::declval<Args>()...); };

		template <typename Adaptor, typename Cond>
		struct sql_partial : sql_adaptor_closure
		{
			Cond cond;

			constexpr sql_partial(Cond c)
				: cond(std::move(c))
			{}

			template <typename T>
			requires partical_adaptor_invocable<Adaptor, T, Cond>
			constexpr auto operator()(T&& t) const
			{
				return Adaptor{}(std::forward<T>(t), cond);
			}
		};

		template <typename T>
		struct sql_adaptor
		{
			template <typename Cond>
			constexpr auto operator()(Cond&& cond) const
			{
				return sql_partial<T, std::decay_t<Cond>>{ std::forward<Cond>(cond) };
			}
		};
	} // namespace sql
} // namespace aquarius