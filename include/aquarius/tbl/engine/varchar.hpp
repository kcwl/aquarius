#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/int_to_string.hpp>
#include <limits>
#include <string_view>

namespace aquarius
{
	namespace tbl
	{
		template <typename T>
		struct _char
		{
			using value_type = T;

			constexpr static auto max_length = std::tuple_size_v<T>;

			static_assert(max_length < std::numeric_limits<uint8_t>::max(), "char size must be less than 256");

			constexpr static auto left_bracket = " char("sv;
			constexpr static auto right_bracket = ")"sv;

			constexpr static auto size = detail::int_to_string<max_length>();

			constexpr static auto name = concat_v<left_bracket, size, right_bracket>;
		};

		template <typename T>
		struct _varchar
		{
			static constexpr auto name = " varchar"sv;

			static constexpr auto max_length = -1;
		};

		template <typename T>
		struct _nvarchar
		{
			static constexpr auto name = " nvarchar"sv;

			static constexpr auto max_length = -1;
		};
	} // namespace tbl
} // namespace aquarius