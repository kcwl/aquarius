#pragma once
#include <type_traits>

namespace  aquarius
{
	template<typename T>
	struct use_empty_value_base
	{
		static constexpr bool value = std::is_empty_v<T> && !std::is_final_v<T>;
	};

	template<typename T>
	static constexpr bool use_empty_value_base_v = use_empty_value_base<T>::value;
}