#pragma once
#include <aquarius/virgo/serialize/scalar_type.hpp>
#include <concepts>
#include <string>
#include <string_view>

namespace aquarius
{
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
	concept pod_t = disjunction_same_as<std::remove_cvref_t<T>, char, bool, float, double, fixed32, fixed64>;

	template <typename T>
	concept reflectable = std::is_aggregate_v<std::remove_cvref_t<T>>;

	template <typename T>
	struct is_map : std::false_type
	{};

	template <typename Key, typename Value>
	struct is_map<std::map<Key, Value>> : std::true_type
	{};

	template <typename T>
	concept map_t = is_map<T>::value;
} // namespace aquarius