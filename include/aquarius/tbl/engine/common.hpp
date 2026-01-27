#pragma once
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{
		struct unique
		{};
		struct primary
		{};

		struct not_null
		{};

		struct default_
		{};

		struct auto_increment
		{};

		struct auto_timestamp
		{};

		struct unsigned_
		{};

		template <typename T>
		struct attribute;

		template <>
		struct attribute<primary>
		{
			constexpr static auto value = "PRIMARY KEY"sv;
		};

		template <>
		struct attribute<unique>
		{
			constexpr static auto value = "UNIQUE"sv;
		};

		template <>
		struct attribute<not_null>
		{
			constexpr static auto value = "NOT NULL"sv;
		};

		template <>
		struct attribute<default_>
		{
			constexpr static auto value = "DEFAULT"sv;
		};

		template <>
		struct attribute<unsigned_>
		{
			constexpr static auto value = "UNSIGNED"sv;
		};

		template <>
		struct attribute<auto_increment>
		{
			constexpr static auto value = "AUTO_INCREMENT"sv;
		};

		template <>
		struct attribute<auto_timestamp>
		{
			constexpr static auto value = "TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP"sv;
		};

		using PK = attribute<primary>;

		using UN = attribute<unsigned_>;

		using AI = attribute<auto_increment>;

		using AT = attribute<auto_timestamp>;

		using DF = attribute<default_>;

		using NN = attribute<not_null>;
	} // namespace tbl
} // namespace aquarius