#pragma once
#include <aquarius/sql/common.hpp>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace sql
	{
		template <typename T>
		struct attribute
		{
			void operator()()
			{
				static_assert(false, "illegal sql attribute type");
			}
		};

		template <>
		struct attribute<not_null>
		{
			constexpr std::string_view operator()()
			{
				return "NOT NULL"sv;
			}
		};

		template <>
		struct attribute<default_>
		{
			template <auto Value>
			constexpr std::string_view operator()()
			{
				return "DEFAULT ("sv + to_string_view<Value>() + ")"sv;
			}
		};

		template <>
		struct attribute<auto_increment>
		{
			constexpr std::string_view operator()()
			{
				return "AUTO_INCREMENT"sv;
			}
		};

		template <>
		struct attribute<auto_timestamp>
		{
			constexpr std::string_view operator()()
			{
				return "DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP"sv;
			}
		};
	} // namespace sql
} // namespace aquarius