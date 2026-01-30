#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/int_to_string.hpp>
#include <limits>
#include <string_view>

namespace aquarius
{
	namespace tbl
	{

		template <bool Dynamic, std::size_t I, typename... Args>
		class basic_char : public fields<Args...>
		{
			static_assert(I < 256, "char length must be less than 256");

		public:
			constexpr static auto get_type_name()
			{
				constexpr static auto char_type_name = " char("sv;
				constexpr static auto varchar_type_name = " varchar("sv;
				constexpr static auto end = ")"sv;

				constexpr static auto number_str = int_to_string<I>();

				return concat_v < Dynamic ? varchar_type_name : char_type_name, number_str, end > ;
			}

		public:
			basic_char() = default;

			basic_char(const std::array<char, I>& v)
				: value_(v.data(), I)
			{}

			basic_char(const std::string& v)
			{
				auto size = v.size() > I - 1 ? I - 1 : v.size();

				value_ = std::string{ v.data(), size };
			}

			basic_char(const char* value)
				: value_(value)
			{}

			operator std::string() const
			{
				return "'" + value_ + "'";
			}

		public:
			void set_value(std::stringstream& ss)
			{
				ss >> value_;
			}

		private:
			std::string value_;
		};

		template <bool Dynamic, std::size_t I, typename... Args>
		std::ostream& operator<<(std::ostream& os, const basic_char<Dynamic, I, Args...>& v)
		{
			return os << static_cast<std::string>(v);
		}

		template <std::size_t I, typename... Args>
		using _char = basic_char<false, I, Args...>;

		template <std::size_t I, typename... Args>
		using varchar = basic_char<true, I, Args...>;
	} // namespace tbl
} // namespace aquarius