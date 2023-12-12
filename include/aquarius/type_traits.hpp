#pragma once
#include <string>
#include <type_traits>
#include <variant>

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

		template <typename _Ty, typename... _Args>
		constexpr static bool is_any_of_v = std::disjunction_v<std::is_same<_Ty, _Args>...>;

		//template <typename _Ty>
		//constexpr static bool is_string_v = is_any_of_v<_Ty, std::string, const char*, const char[], char*>;

		template<typename _Ty>
		struct is_string : std::false_type
		{};

		template<>
		struct is_string<std::string> : std::true_type
		{};

		template <>
		struct is_string<const char*> : std::true_type
		{};

		template <std::size_t N>
		struct is_string<const char(&)[N]> : std::true_type
		{};

		template <>
		struct is_string<char*> : std::true_type
		{};

		template <typename _Ty>
		constexpr static bool is_string_v = is_string<_Ty>::value;


		struct http
		{};

		template<typename _Ty>
		concept integer_t = requires { std::is_integral_v<_Ty>; };

		struct replace_mode
		{};

		struct delete_mode
		{};

		struct select_mode
		{};

		struct create_mode
		{};

		struct remove_mode
		{};

		template <class T>
		struct is_byte : std::false_type
		{};

		template <>
		struct is_byte<std::byte> : std::true_type
		{};

		template <class T>
		static constexpr bool is_byte_v = is_byte<T>::value;

		template <class T>
		struct is_variant : std::false_type
		{};

		template <class... Args>
		struct is_variant<std::variant<Args...>> : std::true_type
		{};

		template <class T>
		static constexpr bool is_variant_v = is_variant<T>::value;

	} // namespace detail
} // namespace aquarius
