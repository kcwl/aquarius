#pragma once
#include <aquarius/core/type_traits.hpp>

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

	template <typename _Ty>
	concept enum_t = std::is_enum_v<std::remove_cvref_t<_Ty>>;

	template<typename _Ty>
	concept boolean_t = std::is_same_v<_Ty, bool>;

	template <typename _Ty, typename... _Args>
	inline constexpr bool is_any_of_v = std::disjunction_v<std::is_same<std::remove_cvref_t<_Ty>, _Args>...>;

	template <typename _Ty>
	concept float_point_t = std::is_floating_point_v<std::remove_reference_t<_Ty>>;

	template <typename _Ty>
	concept integer_t = std::is_integral_v<std::remove_reference_t<_Ty>> && !boolean_t<_Ty>;

	template <typename _Ty>
	concept copable_t = std::is_copy_constructible_v<std::remove_all_extents_t<_Ty>> &&
		std::is_move_constructible_v<std::remove_all_extents_t<_Ty>>;

	template <typename _Ty>
	concept reflection_t =
		copable_t<_Ty> && !std::is_polymorphic_v<_Ty> && (std::is_aggregate_v<_Ty> || std::is_scalar_v<_Ty>);

	template <typename _Ty>
	concept tuple_t = requires() { std::tuple_size<_Ty>(); };

	template <typename _Ty>
	concept class_t = std::is_class_v<std::remove_reference_t<_Ty>>;

	template <typename _Ty>
	concept pod_and_integer_t =
		std::is_trivial_v<std::remove_cvref_t<_Ty>> && std::is_standard_layout_v<std::remove_cvref_t<_Ty>>;

	template <typename _Ty>
	concept struct_t = std::is_aggregate_v<std::remove_cvref_t<_Ty>> && !integer_t<_Ty>;

	template <typename _Ty>
	concept sequence_t = sequence_like<std::remove_cvref_t<_Ty>>::value;

	template <typename _Ty>
	concept string_t = string_like<std::remove_cvref_t<_Ty>>::value;

	template <typename _Ty>
	concept map_t = map_like<_Ty>::value;

	template <typename _Ty>
	concept non_inherit_t = integer_t<_Ty> || /*struct_t<_Ty> ||*/ sequence_t<_Ty> || boolean_t<_Ty> || enum_t<_Ty> ||
		string_t<_Ty> || map_t<_Ty> || float_point_t<_Ty>;

	template <typename _Ty>
	concept inherit_t = !non_inherit_t<_Ty>;

	template<typename _Ty, typename _U>
	concept serialize_object_t = requires (_Ty value, _U& u) 
	{ 
		value.to_binary(u);
		value.from_binary(u);
	};
} // namespace aquarius
