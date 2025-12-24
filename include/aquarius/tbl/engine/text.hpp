#pragma once
#include <cstdint>
#include <limits>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{
		template <typename T>
		struct vtext;

		template <>
		struct vtext<uint8_t>
		{
			static constexpr std::string_view name = " tinytext"sv;

			static constexpr auto max_length = std::numeric_limits<uint8_t>::max();
		};

		template <>
		struct vtext<uint16_t>
		{
			static constexpr std::string_view name = " text"sv;

			static constexpr auto max_length = std::numeric_limits<uint16_t>::max();
		};

		template <>
		struct vtext<uint32_t>
		{
			static constexpr std::string_view name = " mediumtext"sv;

			static constexpr auto max_length = std::numeric_limits<uint32_t>::max();
		};

		template <>
		struct vtext<uint64_t>
		{
			static constexpr std::string_view name = " longtext"sv;

			static constexpr auto max_length = std::numeric_limits<uint64_t>::max();
		};

		template <typename T>
		struct blob_impl;

		template <>
		struct blob_impl<uint8_t>
		{
			static constexpr std::string_view name = " tinyblob"sv;

			static constexpr auto max_length = std::numeric_limits<uint8_t>::max();
		};

		template <>
		struct blob_impl<uint16_t>
		{
			static constexpr std::string_view name = " blob"sv;

			static constexpr auto max_length = std::numeric_limits<uint16_t>::max();
		};

		template <>
		struct blob_impl<uint32_t>
		{
			static constexpr std::string_view name = " mediumblob"sv;

			static constexpr auto max_length = std::numeric_limits<uint32_t>::max();
		};

		template <>
		struct blob_impl<uint64_t>
		{
			static constexpr std::string_view name = " longblob"sv;

			static constexpr auto max_length = std::numeric_limits<uint64_t>::max();
		};
	} // namespace tbl
} // namespace aquarius