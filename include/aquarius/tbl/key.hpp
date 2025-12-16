#pragma once
#include <aquarius/sql/common.hpp>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{
		template <typename T>
		struct key
		{
			void operator()()
			{
				static_assert(false, "illegal sql k;ey type");
			}
		};

		template <>
		struct key<primary>
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
	} // namespace sql
} // namespace aquarius