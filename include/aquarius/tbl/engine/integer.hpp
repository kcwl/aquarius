#pragma once
#include <cstdint>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{
		template <auto Value>
		struct integer_impl;

		template <>
		struct integer_impl<1>
		{
			using value_type = uint8_t;

			static constexpr value_type filter = 0xff;

			static constexpr std::string_view name = "tinyint"sv;
		};

		template <>
		struct integer_impl<2>
		{
			using value_type = uint16_t;

			static constexpr value_type filter = 0xffff;

			static constexpr std::string_view name = "smallint"sv;
		};

		template <>
		struct integer_impl<3>
		{
			using value_type = uint32_t;

			static constexpr value_type filter = 0xffffff;

			static constexpr std::string_view name = "mediumint"sv;
		};

		template <>
		struct integer_impl<4>
		{
			using value_type = uint32_t;

			static constexpr value_type filter = 0xffffffff;

			static constexpr std::string_view name = "int"sv;
		};

		template <>
		struct integer_impl<8>
		{
			using value_type = int64_t;

			static constexpr value_type filter = 0xffffffffffffffff;

			static constexpr std::string_view name = "bigint"sv;
		};

		template <auto Value>
		class integer
		{
		public:
			constexpr static auto name = integer_impl<Value>::name;

			using value_type = typename integer_impl<Value>::value_type;

		public:
			integer() = default;

			template <typename T>
			requires(sizeof(T) <= Value)
			integer(T value)
				: value_(static_cast<char>(value))
			{}

			integer(const integer& i)
				: value_(i.value_)
			{}

			integer(integer&& i) noexcept
				: value_(std::exchange(i.value_, 0))
			{}

			integer& operator=(const integer& i)
			{
				if (this != std::addressof(i))
				{
					value_ = i.value_;
				}

				return *this;
			}

			integer& operator=(integer&& i) noexcept
			{
				if (this != std::addressof(i))
				{
					value_ = std::exchange(i.value_, 0);
				}

				return *this;
			}

			template <typename T>
			requires(sizeof(T) <= Value)
			integer& operator=(T value)
			{
				value_ = static_cast<char>(value);
				return *this;
			}

			template <typename T>
			requires(sizeof(T) <= Value)
			integer& operator=(T&& value) noexcept
			{
				value_ = std::move(value);
				return *this;
			}

			operator value_type() const
			{
				return static_cast<value_type>(value_ & integer_impl<Value>::filter);
			}

		private:
			value_type value_;
		};

		using int1 = integer<1>;
		using int2 = integer<2>;
		using int3 = integer<3>;
		using int4 = integer<4>;
		using int8 = integer<8>;
	} // namespace sql
} // namespace aquarius