#pragma once
#include <aquarius/serialize/scalar_type.hpp>
#include <concepts>
#include <string>
#include <string_view>

namespace aquarius
{
	template <typename T>
	struct remove_member_pointer
	{
		using type = T;
	};

	template <typename R, typename T>
	struct remove_member_pointer<T(R::*)>
	{
		using type = T;
	};

	template <typename T>
	struct member_pointer_struct
	{
		using type = T;
	};

	template <typename R, typename T>
	struct member_pointer_struct<T(R::*)>
	{
		using type = R;
	};

	template <typename T>
	concept iterator_t = requires() {
		std::begin(std::declval<T>());
		std::end(std::declval<T>());
	};

	template <typename T>
	concept push_t = requires() {
		std::declval<T>().begin();
		std::declval<T>().end();
		std::back_inserter(std::declval<std::add_lvalue_reference_t<T>>());
	};

	template <typename T, typename... Args>
	concept disjunction_same_as = std::disjunction_v<std::is_same<std::remove_cvref_t<T>, Args>...>;

	template <typename T>
	concept integer_t = disjunction_same_as<T, uint32_t, uint64_t>;

	template <typename T>
	struct is_vector : std::false_type
	{};

	template <typename T, typename Alloc>
	struct is_vector<std::vector<T, Alloc>> : public std::true_type
	{};

	template <typename T>
	struct is_string : std::false_type
	{};

	template <>
	struct is_string<std::string> : public std::true_type
	{};

	template <>
	struct is_string<std::string_view> : public std::true_type
	{};

	template <typename T>
	concept boolean = std::same_as<T, bool>;

	template <typename T>
	concept string_t = is_string<std::remove_cvref_t<T>>::value;

	template <typename T>
	concept repeated_t = is_vector<std::remove_cvref_t<T>>::value;

	template <typename T>
	concept zig_zag = disjunction_same_as<T, int32_t, int64_t>;

	template <typename T>
	concept fixed_t = disjunction_same_as<T, fixed32, fixed64>;

	template <typename T>
	concept pod_t = disjunction_same_as<std::remove_cvref_t<T>, char, bool, float, double>;

	template <typename T>
	concept reflectable = std::is_aggregate_v<std::remove_cvref_t<T>> && !fixed_t<T>;

	template <typename T>
	struct is_map : std::false_type
	{};

	template <typename Key, typename Value>
	struct is_map<std::map<Key, Value>> : std::true_type
	{};

	template <typename T>
	concept map_t = is_map<T>::value;
} // namespace aquarius