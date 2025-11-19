#pragma once
#include <concepts>

namespace aquarius
{
	template<typename T>
	struct value_to_tag {};


	inline void tag_invoke(value_to_tag<std::nullopt_t>, std::nullopt_t)
	{
		return;
	}


	template<typename T, typename P, typename = void>
	struct has_tag_invokable : std::false_type {};

	template<typename T, typename P>
	struct has_tag_invokable<T, P, decltype(tag_invoke(value_to_tag<T>(), std::declval<P&>()))> : std::true_type {};

	template<typename T, typename P>
	inline void value_to(P& t)
	{
		constexpr bool b = has_tag_invokable<T, P>::value;

		if constexpr (b)
		{
			tag_invoke(value_to_tag<T>(), t);
		}
	}

} // namespace aquarius