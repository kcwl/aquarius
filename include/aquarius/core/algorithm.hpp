#pragma once
#include <algorithm>
#include "../deps/reflect/include/reflect.hpp"

namespace aquarius
{
	namespace core
	{
		template<typename _Ty, typename _Func, std::size_t... I>
		constexpr void for_each_impl(_Ty&& val, _Func func, std::index_sequence<I...>)
		{
			return (func(reflect::get<I>(std::forward<_Ty>(val))), ...);
		}

		template<typename _Ty, typename _Func, std::size_t N = reflect::rf_size_v<_Ty>, typename Indices = std::make_index_sequence<N>>
		constexpr void for_each(_Ty&& val, _Func func)
		{
			return for_each_impl(std::forward<_Ty>(val), func, Indices{});
		}
	}
}