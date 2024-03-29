#pragma once
#include <string>
#include <type_traits>
#include <variant>

namespace aquarius
{
	namespace system
	{
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

		// template <typename _Ty>
		// constexpr static bool is_string_v = is_any_of_v<_Ty, std::string, const char*, const char[], char*>;

		template <typename _Ty>
		struct is_string : std::false_type
		{};

		template <>
		struct is_string<std::string> : std::true_type
		{};

		template <>
		struct is_string<const char*> : std::true_type
		{};

		template <std::size_t N>
		struct is_string<const char (&)[N]> : std::true_type
		{};

		template <>
		struct is_string<char*> : std::true_type
		{};

		template <typename _Ty>
		constexpr static bool is_string_v = is_string<_Ty>::value;

		struct http
		{};

		template <typename _Ty>
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

		template <typename _Ty, std::size_t N>
		concept aggregate_inialize_v = is_aggregate_initialize<_Ty, N>::value;

		template <typename _Ty>
		concept tuple_t = requires() { std::tuple_size<_Ty>(); };

		template <typename _Ty>
		concept class_t = std::is_class_v<std::remove_reference_t<_Ty>>;

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

	} // namespace system

} // namespace aquarius
