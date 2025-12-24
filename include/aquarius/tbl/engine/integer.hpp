#pragma once
#include <cstdint>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{

		template <typename T>
		struct integer;

		template <>
		struct integer<uint8_t>
		{
			using value_type = uint8_t;

			static constexpr auto max_length = std::numeric_limits<uint8_t>::max();

			static constexpr value_type filter = 0xff;

			static constexpr std::string_view name = " tinyint"sv;
		};

		template <>
		struct integer<uint16_t>
		{
			using value_type = uint16_t;

			static constexpr auto max_length = std::numeric_limits<uint16_t>::max();

			static constexpr value_type filter = 0xffff;

			static constexpr std::string_view name = " smallint"sv;
		};

		template <>
		struct integer<int32_t>
		{
			using value_type = int32_t;

			static constexpr auto max_length = std::numeric_limits<int32_t>::max();

			static constexpr value_type filter = 0xffffff;

			static constexpr std::string_view name = " mediumint"sv;
		};

		template <>
		struct integer<uint32_t>
		{
			using value_type = uint32_t;

			static constexpr auto max_length = std::numeric_limits<uint32_t>::max();

			static constexpr value_type filter = 0xffffffff;

			static constexpr std::string_view name = " int"sv;
		};

		template <>
		struct integer<uint64_t>
		{
			using value_type = int64_t;

			static constexpr auto max_length = std::numeric_limits<uint64_t>::max();

			static constexpr value_type filter = 0xffffffffffffffff;

			static constexpr std::string_view name = " bigint"sv;
		};
	} // namespace tbl
} // namespace aquarius