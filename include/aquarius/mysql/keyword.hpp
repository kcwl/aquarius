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

	inline constexpr std::string_view COMMA = ","sv;

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

	template <std::size_t N>
	struct string_literal
	{
		constexpr string_literal(const char (&arr)[N])
		{
			std::copy_n(arr, N, value);
		}

		char value[N];
	};

	template <string_literal param>
	struct bind_param
	{
		static constexpr std::string_view value = param.value;
	};

} // namespace aquarius
#define AQUARIUS_CAT1(a, b) a##_##b
#define AQUARIUS_CAT(a, b) AQUARIUS_CAT1(a,b)

#define AQUARIUS_RSEQ_N()                                                                                              \
	31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define AQUARIUS_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, \
					   _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, N, ...)                                       \
	N

#define AQUARIUS_NARG_(...) AQUARIUS_ARG_N(##__VA_ARGS__)
#define AQUARIUS_NARG(...) AQUARIUS_NARG_(__VA_ARGS__, AQUARIUS_RSEQ_N())

#define AQUARIUS_SQL_BIND_IMPL_1(element) aquarius::bind_param<#element>::value
#define AQUARIUS_SQL_BIND_IMPL_2(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_1(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_3(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_2(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_4(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_3(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_5(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_4(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_6(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_5(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_7(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_6(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_8(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_7(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_9(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_8(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_10(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_9(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_11(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_10(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_12(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_11(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_13(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_12(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_14(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_13(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_15(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_14(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_16(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_15(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_17(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_16(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_18(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_17(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_19(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_18(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_20(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_19(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_21(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_20(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_22(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_21(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_23(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_22(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_24(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_23(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_25(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_24(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_26(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_25(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_27(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_26(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_28(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_27(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_29(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_28(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_30(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_29(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_31(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_30(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND_IMPL_32(element, ...) aquarius::bind_param<#element>::value, AQUARIUS_SQL_BIND_IMPL_31(##__VA_ARGS__)
#define AQUARIUS_SQL_BIND(...) AQUARIUS_CAT(AQUARIUS_SQL_BIND_IMPL, AQUARIUS_NARG(##__VA_ARGS__))(##__VA_ARGS__)