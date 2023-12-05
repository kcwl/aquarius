#pragma once
#include <string>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	inline constexpr std::string_view SELECT = "select"sv;

	inline constexpr std::string_view SPACE = " "sv;

	inline constexpr std::string_view ASTERISK = "*"sv;

	inline constexpr std::string_view FROM = "from"sv;

	inline constexpr std::string_view SEPARATOR = ";"sv;

	inline constexpr std::string_view INSERT = "insert"sv;

	inline constexpr std::string_view INTO = "into"sv;

	inline constexpr std::string_view VALUES = "values"sv;

	inline constexpr std::string_view LEFT_BRACKET = "("sv;

	inline constexpr std::string_view RIGHT_BRACKET = ")"sv;

	inline constexpr std::string_view REMOVE = "delete"sv;

	inline constexpr std::string_view WHERE = "where"sv;

	inline constexpr std::string_view LESS = "<"sv;

	inline constexpr std::string_view EQUAL = "="sv;

	inline constexpr std::string_view GREATER = ">"sv;

	inline constexpr std::string_view NOT = "!"sv;

	inline constexpr std::string_view THIRD_WAY = "<=>"sv;

	inline constexpr std::string_view OR = "or";

	inline constexpr std::string_view AND = "and";

	inline constexpr std::string_view IS_NULL = "is null";

	inline constexpr std::string_view BETWEEN = "between";

	inline constexpr std::string_view SET = "set"sv;

	inline constexpr std::string_view UPDATE = "update"sv;

	inline constexpr std::string_view REPLACE = "replace"sv;

	template <class T>
	struct indentify
	{};

	constexpr auto mysql_type(indentify<std::string>)
	{
		return "varchar(255)";
	}

	constexpr auto mysql_type(indentify<int>)
	{
		return "int(11)";
	}

	template <class T>
	constexpr auto mysql_type(indentify<T>)
	{
		return mysql_type(indentify<std::string>{});
	}

	constexpr auto mysql_type(indentify<uint64_t>)
	{
		return "bigint";
	}

	constexpr auto mysql_type(indentify<bool>)
	{
		return "int";
	}

	constexpr auto mysql_type(indentify<time_t>)
	{
		return "bigint";
	}

	constexpr auto mysql_type(indentify<std::byte>)
	{
		return "bit";
	}
} // namespace aquarius