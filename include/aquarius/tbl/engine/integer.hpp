#pragma once
#include <aquarius/tbl/engine/field.hpp>
#include <cstdint>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{
		template <std::size_t I, typename... Args>
		class integer : public field<Args...>
		{
		public:
			constexpr static auto get_type_name()
			{
				if constexpr (I <= 1)
				{
					return " tinyint"sv;
				}
				else if constexpr (I == 2)
				{
					return " smallint"sv;
				}
				else if constexpr (I == 3)
				{
					return " mediumint"sv;
				}
				else if constexpr (I == 4)
				{
					return " int"sv;
				}
				else if constexpr (I <= 8)
				{
					return " bigint";
				}
			}

		public:
			integer() = default;

			integer(uint64_t v)
				: value_(v)
			{}

			integer(const integer& other)
				: value_(other.value_)
			{}

			integer(integer&& other) noexcept
				: value_(std::exchange(other.value_, 0))
			{}

			integer& operator=(const integer& other)
			{
				if (this != &other)
				{
					value_ = other.value_;
				}

				return *this;
			}

			integer& operator=(integer&& other) noexcept
			{
				if (this != &other)
				{
					value_ = std::exchange(other.value_, 0);
				}

				return *this;
			}

			static_assert(I < 8, "integer requires I less than 8");

			void set_value(std::stringstream& ss)
			{
				ss >> value_;
			}

		public:
			operator uint64_t() const
			{
				return value_;
			}

		private:
			uint64_t value_;
		};
	} // namespace tbl
} // namespace aquarius