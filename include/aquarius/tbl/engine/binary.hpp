#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/tbl/engine/field.hpp>
#include <cstdint>
#include <limits>
#include <string_view>

using namespace std::string_view_literals;

namespace aquarius
{
	namespace tbl
	{
		template <bool Dynamic, std::size_t I, typename... Args>
		class basic_binary : public fields<Args...>
		{
		public:
			constexpr static auto get_type_name()
			{
				constexpr static auto binary_type_name = " binary("sv;
				constexpr static auto varbinary_type_name = " varbinary("sv;
				constexpr static auto end = ")"sv;

				constexpr static auto number_str = int_to_string<I>();

				return concat_v < Dynamic ? varbinary_type_name : binary_type_name, number_str, end > ;
			}

		public:
			basic_binary() = default;

			template <typename T>
			requires std::convertible_to<T, uint8_t>
			basic_binary(const std::vector<uint8_t>& v)
				: basic_binary(v)
			{}

			template <typename T>
			requires std::convertible_to<T, uint8_t>
			basic_binary(std::initializer_list<T> v)
				: value_()
			{
				auto size = v.size() > I ? I : v.size();

				std::copy_n(v.begin(), size, std::back_inserter(value_));
			}

			std::ostream& operator<<(std::ostream& os) const
			{
				for (auto& s : value_)
				{
					os << (int)s;
				}

				return os;
			}

			void set_value(std::stringstream& ss)
			{
				ss >> value_;
			}

		private:
			std::vector<uint8_t> value_;
		};

		template <bool Dynamic, std::size_t I, typename... Args>
		std::ostream& operator<<(std::ostream& os, const basic_binary<Dynamic, I, Args...>& v)
		{
			v << os;

			return os;
		}

		template <std::size_t I, typename... Args>
		using binary = basic_binary<false, I, Args...>;

		template <std::size_t I, typename... Args>
		using varbinary = basic_binary<true, I, Args...>;

	} // namespace tbl
} // namespace aquarius