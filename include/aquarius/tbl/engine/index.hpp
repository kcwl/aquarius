#pragma once
#include <aquarius/sql/common.hpp>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{
		template <typename T>
		struct index
		{
			constexpr std::string_view operator()()
			{
				return "INDEX"sv;
			}
		};

		template <>
		struct index<primary>
		{
			constexpr std::string_view operator()()
			{
				return "PRIMARY INDEX"sv;
			}
		};

		template <>
		struct index<unique>
		{
			constexpr std::string_view operator()()
			{
				return "UNIQUE INDEX"sv;
			}
		};
	} // namespace sql
} // namespace aquarius