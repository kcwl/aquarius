#pragma once
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace detail
	{
		template <typename T>
		inline constexpr std::string_view struct_name()
		{
			using namespace std::string_view_literals;

#ifndef __linux
			constexpr std::string_view name = __FUNCSIG__ ""sv;

			constexpr auto left_bracket = name.find_last_of("<");

			constexpr auto end_bracket = name.find_last_of(">");

			constexpr auto temp_name = name.substr(left_bracket + 1, end_bracket - left_bracket - 1);

			constexpr auto start = temp_name.find_last_of(" ");

			if constexpr (start == std::string_view::npos)
			{
				return temp_name;
			}
			else
			{
				constexpr auto namespace_name = temp_name.substr(start + 1, end_bracket - start - 1);

				constexpr auto last = namespace_name.find_last_of(":");

				if constexpr (last != std::string_view::npos)
					return namespace_name.substr(last + 1);
				else
				{
					return namespace_name;
				}
			}
#else
			constexpr std::string_view name = __PRETTY_FUNCTION__;

			constexpr auto left_bracket = name.find_last_of("[");
			constexpr auto right_bracket = name.find_last_of("]");
			constexpr auto name_in_bracket = name.substr(left_bracket + 1, right_bracket - left_bracket - 1);

			constexpr auto left_equ = name_in_bracket.find_first_of("=");
			constexpr auto right_f = name_in_bracket.find_first_of(";");

			constexpr auto first_name = name_in_bracket.substr(left_equ + 2, right_f - left_equ - 2);

			constexpr auto sp = first_name.find_last_of(":");

			if constexpr (sp == std::string_view::npos)
			{
				return first_name;
			}
			else
			{
				return first_name.substr(sp + 1);
			}
#endif
		}
	} // namespace detail
} // namespace aquarius