#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include <map>

namespace aquarius
{
	template <typename _Ty>
	struct string_like : std::false_type
	{};

	template <>
	struct string_like<std::string> : std::true_type
	{};

	template <>
	struct string_like<char*> : std::true_type
	{};

	template <>
	struct string_like<const char*> : std::true_type
	{};

	template <typename _Ty, std::size_t N>
	struct string_like<_Ty[N]> : std::true_type
	{};

	template <typename _Ty>
	struct sequence_like : std::false_type
	{};

	template <typename _Ty>
	struct sequence_like<std::vector<_Ty>> : std::true_type
	{};

	template <typename _Ty>
	struct map_like : public std::false_type
	{};

	template <typename _Key, typename _Value>
	struct map_like<std::map<_Key, _Value>> : std::true_type
	{};
}