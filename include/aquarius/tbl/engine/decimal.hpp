#pragma once
#include <aquarius/logger.hpp>
#include <string>

namespace aquarius
{
	namespace tbl
	{

		template <std::size_t Significant, std::size_t Decimal, typename... Args>
		class decimal : public field<Args...>
		{
			static_assert(Significant < 65, "decimal significant must be less than 65");

		public:
			constexpr static auto get_type_name()
			{
				constexpr static auto type_name = " decimal("sv;
				constexpr static auto sp = ","sv;
				constexpr static auto end = ")"sv;

				constexpr static auto num_str_dec = int_to_string<Decimal>();
				constexpr static auto num_str_sign = int_to_string<Significant>();

				return concat_v<type_name, num_str_sign, sp, num_str_dec, end>;
			}

		public:
			decimal() = default;

			decimal(const char* v)
				: decimal(std::string(v))
			{}

			decimal(const std::string& v)
				: value_()
			{
				assign(v);
			}

			template <typename T>
			requires std::is_floating_point_v<T>
			decimal(T v)
				: decimal(std::to_string(v))
			{}

			decimal(const decimal& d)
				: value_(d.value_)
			{}

			decimal(decimal&& d) noexcept
				: value_(std::move(d.value_))
			{}

			decimal& operator=(const decimal& d)
			{
				if (this != std::addressof(d))
				{
					value_ = d.value_;
				}

				return *this;
			}

			decimal& operator=(decimal&& d) noexcept
			{
				if (this != std::addressof(d))
				{
					value_ = std::move(d.value_);
				}

				return *this;
			}

		public:
			operator std::string() const
			{
				return value_;
			}

		private:
			void assign(const std::string& v)
			{
				auto val = big_add(v);

				auto pos = val.find('.');

				std::size_t significant = pos != std::string::npos ? pos + 1 : val.size();

				if (significant > Significant)
				{
					XLOG_ERROR() << "decimal significant must be less than " << Significant;
					return;
				}

				auto basic_decimal = val.size() - pos - 1;

				if (basic_decimal > Decimal)
				{
					XLOG_ERROR() << "decimal decimal must be less than " << Decimal;
					return;
				}

				value_ = val;
			}

			std::string big_add(const std::string& value)
			{
				auto pos = value.find('.');
				if (pos == std::string::npos)
				{
					return value;
				}

				auto basic_decimal = value.size() - pos - 1;

				if (basic_decimal < Decimal)
					return value;

				auto integer = value.at(pos + Decimal + 1);

				if (integer < '5')
					return value;

				auto result = value.substr(0, pos + Decimal);

				bool carry = false;
				for (auto iter = result.rbegin(); iter != result.rend(); ++iter)
				{
					if (*iter == '.')
						continue;

					auto val = *iter - '0' + carry;

					if (val < 10)
					{
						*iter = val + '0';
						break;
					}
					else
					{
						*iter = '0';
					}

					carry = !carry;
				}

				if (carry)
				{
					result.insert(0, "1");
				}

				return result;
			}

			void set_value(std::stringstream& ss)
			{
				ss >> value_;
			}

		private:
			std::string value_;
		};
	} // namespace tbl
} // namespace aquarius

template <std::size_t Significant, std::size_t Decimal, typename... Args>
std::ostream& operator<<(std::ostream& os, const aquarius::tbl::decimal<Significant, Decimal, Args...>& v)
{
	os << static_cast<std::string>(v);
	return os;
}