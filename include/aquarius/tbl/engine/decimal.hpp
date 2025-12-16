#pragma once
#include <aquarius/logger.hpp>
#include <string>

namespace aquarius
{
	namespace tbl
	{
		template <std::size_t Significant = 10, std::size_t Decimal = 0>
		class basic_decimal
		{
			static_assert(Significant < 65, "decimal significant must be less than 65");

		public:
			basic_decimal() = default;

			basic_decimal(const char* v)
				: basic_decimal(std::string(v))
			{}

			basic_decimal(const std::string& v)
				: value()
			{
				assign(v);
			}

			basic_decimal(const basic_decimal& d)
				: value(d.value)
			{}

			basic_decimal(basic_decimal&& d) noexcept
				: value(std::move(d.value))
			{}

			basic_decimal& operator=(const basic_decimal& d)
			{
				if (this != std::addressof(d))
				{
					value = d.value;
				}

				return *this;
			}

			basic_decimal& operator=(basic_decimal&& d) noexcept
			{
				if (this != std::addressof(d))
				{
					value = std::move(d.value);
				}

				return *this;
			}

		public:
			operator std::string()
			{
				return value;
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

				value = val;
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

		private:
			std::string value;
		};
	} // namespace sql
} // namespace aquarius