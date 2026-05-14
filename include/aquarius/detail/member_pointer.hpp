#pragma once
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	template <typename T, auto Ptr>
	constexpr static auto get_member_pointer_name()
	{
#ifdef _WIN32
		constexpr std::string_view func_line = __FUNCSIG__ ""sv;

		constexpr auto start_pos = func_line.find_last_of(':');

		return func_line.substr(start_pos + 1, func_line.find_last_of('>') - start_pos - 1);
#else
		constexpr std::string_view func_line = __PRETTY_FUNCTION__;

		constexpr auto start_pos = func_line.find_last_of(':');

		return func_line.substr(start_pos + 1, func_line.find_last_of(']') - start_pos - 1);
#endif
	}

	template <typename T, auto Ptr>
	struct member_pointer_name
	{
		constexpr static auto value = get_member_pointer_name<T, Ptr>();
	};
} // namespace aquarius