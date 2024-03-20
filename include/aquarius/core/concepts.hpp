#pragma once
#include <aquarius/core/impl/type_traits.hpp>

namespace aquarius
{
	template <typename _Ty>
	struct function_traits;

	template <typename _Return, typename _Args>
	struct function_traits<_Return(_Args)>
	{
		using type = _Args;
	};

	template <typename _Return, typename _Func, typename _Args>
	struct function_traits<_Return (_Func::*)(_Args) const> : function_traits<_Return(_Args)>
	{};

	template <typename _Func>
	struct function_traits : function_traits<decltype(&_Func::operator())>
	{};

	template <typename _Ty>
	concept swap_t = requires(_Ty value) { value.swap(value); };
} // namespace aquarius
