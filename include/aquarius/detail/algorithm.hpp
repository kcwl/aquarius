#pragma once
#include <algorithm>
#include <boost/pfr.hpp>

namespace aquarius
{
	namespace detail
	{
		template<typename _Ty, typename Func, std::size_t... I>
		constexpr void for_each_impl(_Ty&& val, Func func, std::index_sequence<I...>)
		{
			return (func(boost::pfr::get<I>(std::forward<_Ty>(val))), ...);
		}

		template<typename _Ty, typename Func, std::size_t N = boost::pfr::tuple_size_v<_Ty>, typename Indices = std::make_index_sequence<N>>
		constexpr void for_each(_Ty&& val, Func func)
		{
			return for_each_impl(std::forward<_Ty>(val), func, Indices{});
		}
	}
}