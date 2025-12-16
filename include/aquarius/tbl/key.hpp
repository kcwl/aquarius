#pragma once
#include <aquarius/tbl/common.hpp>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{
		template <typename T>
		struct key
		{
			constexpr std::string_view operator()()
			{
				return "PRIMARY KEY"sv;
			}
		};

		template <>
		struct key<foreign>
		{
			constexpr std::string_view operator()()
			{
				return "FOREIGN KEY"sv;
			}
		};

		template <>
		struct key<unique>
		{
			constexpr std::string_view operator()()
			{
				return "UNIQUE"sv;
			}
		};
	} // namespace tbl
} // namespace aquarius