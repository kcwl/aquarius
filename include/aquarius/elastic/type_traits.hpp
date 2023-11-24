#pragma once
#include "fixed.hpp"

#include <forward_list>
#include <list>
#include <string>
#include <type_traits>
#include <vector>
#include <map>

namespace aquarius
{
	namespace impl
	{
		struct any_type
		{
			std::size_t ignore_;

			template <typename _Ty>
			constexpr operator _Ty() const noexcept
			{
				return _Ty{};
			};
		};
	} // namespace impl

	template <typename _Ty, typename Indices, typename = void>
	struct is_aggregate_initalize_impl : std::false_type
	{};

	template <typename _Ty, size_t... I>
	struct is_aggregate_initalize_impl<_Ty, std::index_sequence<I...>,
									   std::void_t<decltype(_Ty{ impl::any_type{ I }... })>> : std::true_type
	{};

	template <typename _Ty, std::size_t N>
	struct is_aggregate_initialize : is_aggregate_initalize_impl<_Ty, std::make_index_sequence<N>>
	{};

	template <typename _Ty>
	struct is_fixed : std::false_type
	{};

	template <typename _Ty>
	struct is_fixed<fixed<_Ty>> : std::true_type
	{};

	template <typename _Ty, typename... _Args>
	inline constexpr bool is_any_of_v = std::disjunction_v<std::is_same<std::remove_cvref_t<_Ty>, _Args>...>;

	template <typename _Ty>
	struct zig_zag
	{
		using type = _Ty;
	};

	template <>
	struct zig_zag<int8_t>
	{
		using type = uint8_t;
	};
	template <>

	struct zig_zag<uint8_t>
	{
		using type = int8_t;
	};

	template <>
	struct zig_zag<int16_t>
	{
		using type = uint16_t;
	};

	template <>
	struct zig_zag<uint16_t>
	{
		using type = int16_t;
	};

	template <>
	struct zig_zag<int32_t>
	{
		using type = uint32_t;
	};
	template <>
	struct zig_zag<uint32_t>
	{
		using type = int32_t;
	};

	template <>
	struct zig_zag<int64_t>
	{
		using type = uint64_t;
	};

	template <>
	struct zig_zag<uint64_t>
	{
		using type = int64_t;
	};

	template <typename _Ty>
	using zig_zag_t = typename zig_zag<std::remove_cvref_t<_Ty>>::type;

	template <typename _Ty, std::size_t N>
	concept aggregate_inialize_t = is_aggregate_initialize<_Ty, N>::value;

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
	concept signed_numric_t = is_any_of_v<_Ty, int8_t, int16_t, int32_t, int64_t>;

	template <typename _Ty>
	concept unsigned_numric_t = std::is_unsigned_v<std::remove_cvref_t<_Ty>>;

	template <typename _Ty>
	concept other_numric_t = is_any_of_v<std::remove_cvref_t<_Ty>, bool, std::byte, char> ||
							 std::is_enum_v<std::remove_cvref_t<_Ty>> || unsigned_numric_t<_Ty>;

	template <typename _Ty>
	concept multi_numric_v = is_any_of_v<_Ty, double, float>;

	template <typename _Ty>
	concept varint_t = signed_numric_t<std::remove_cvref_t<_Ty>> || other_numric_t<std::remove_cvref_t<_Ty>>;

	template <typename _Ty>
	concept integer_t = signed_numric_t<_Ty> || other_numric_t<_Ty> || multi_numric_v<_Ty>;

	template <typename _Ty>
	concept fixed_t = is_fixed<_Ty>::value || multi_numric_v<_Ty>;

	template <typename _Ty>
	concept optional_t = requires(_Ty value) {
		value.has_value();
		*value;
	};

	template <typename _Ty>
	concept pod_t = std::is_trivial_v<std::remove_cvref_t<_Ty>> &&
					std::is_standard_layout_v<std::remove_cvref_t<_Ty>> && !integer_t<std::remove_cvref_t<_Ty>>;

	template <typename _Ty>
	concept sequence_t = requires(_Ty value) {
		value.size();
		std::begin(value);
		std::end(value);
		value.data();
		typename std::remove_cvref_t<_Ty>::value_type;
	};

	template<typename _Ty>
	struct is_map : public std::false_type
	{};

	template<typename _Key, typename _Value>
	struct is_map<std::map<_Key, _Value>> : std::true_type
	{};

	template<typename _Ty>
	concept map_t = is_map<_Ty>::value;

	template <typename _Ty>
	concept non_inherit_t = integer_t<_Ty> || pod_t<_Ty> || sequence_t<_Ty> || optional_t<_Ty> || fixed_t<_Ty> || map_t<_Ty>;

	template <typename _Ty>
	concept inherit_t = !non_inherit_t<_Ty>;

} // namespace elastic