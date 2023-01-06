#pragma once
#include <string>
#include <type_traits>

namespace aquarius
{
	namespace detail
	{
		template <typename _Ty>
		struct use_empty_value_base
		{
			static constexpr bool value = std::is_empty_v<_Ty> && !std::is_final_v<_Ty>;
		};

		template <typename _Ty>
		static constexpr bool use_empty_value_base_v = use_empty_value_base<_Ty>::value;

		template <typename _Ty>
		struct is_pod : std::false_type
		{};

		template <typename _Ty>
		requires(std::is_standard_layout_v<_Ty> && std::is_trivial_v<_Ty>)
		struct is_pod<_Ty> : std::true_type
		{};

		template <typename _Ty>
		constexpr static bool is_pod_v = is_pod<std::remove_cvref_t<_Ty>>::value;

		template <typename _Ty, typename _ = std::void_t<>>
		struct is_container : std::false_type
		{};

		template <typename _Ty>
		struct is_container<
			_Ty, std::void_t<typename _Ty::value_type, typename _Ty::size_type, typename _Ty::allocator_type,
							 typename _Ty::iterator, typename _Ty::const_iterator, decltype(std::declval<_Ty>().size()),
							 decltype(std::declval<_Ty>().begin()), decltype(std::declval<_Ty>().end()),
							 decltype(std::declval<_Ty>().cbegin()), decltype(std::declval<_Ty>().cend())>>
			: std::true_type
		{};

		template <typename _Ty>
		constexpr static bool is_container_v = is_container<_Ty>::value;

		template <typename _Ty>
		struct is_string : std::false_type
		{};

		template <>
		struct is_string<std::string> : std::true_type
		{};
	} // namespace detail
} // namespace aquarius
