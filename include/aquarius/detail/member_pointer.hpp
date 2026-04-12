#pragma once
#include <string_view>

namespace aquarius
{
	template <typename T, auto Ptr>
	constexpr static auto get_member_pointer_name()
	{
#ifdef _WIN32
		constexpr static std::string_view func_line = __FUNCSIG__""sv;

		constexpr static auto start_pos = func_line.find_last_of(':');

		constexpr static auto name = func_line.substr(start_pos + 1, func_line.find_last_of('>') - start_pos - 1);
#else
		constexpr static std::string_view func_line = __PRETTY_FUNCTION__;

		constexpr static auto start_pos = func_line.find_last_of(':');

		constexpr static auto name = func_line.substr(start_pos + 1, func_line.find_last_of(']') - start_pos - 1);
#endif
		return name;
	}

	template<typename T, auto Ptr>
	struct member_pointer_name
	{
		constexpr static auto value = get_member_pointer_name<T, Ptr>();
	};
} // namespace aquarius