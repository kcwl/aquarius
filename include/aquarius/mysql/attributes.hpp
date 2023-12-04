#pragma once
#include <string>
#include <string_view>
#include <aquarius/mysql/sql_keyword.hpp>
#include <aquarius/mysql/algorithm.hpp>

namespace aquarius
{
	template <typename _Ty, const std::string_view& value>
	class attributes
	{
		static constexpr std::string_view sql_begin = value;
	public:
		attributes() =default;

		~attributes() = default;

	public:
		attributes& operator==(_Ty&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, EQUAL, EQUAL, SPACE>;

			attr_str_ = sql + std::to_string(t);

			return *this;
		}

		attributes& operator!=(_Ty&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, NOT, EQUAL, SPACE>;

			attr_str_ = sql + std::to_string(t);

			return *this;
		}

		attributes& operator<(_Ty&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, LESS, SPACE>;

			attr_str_ = sql + std::to_string(t);

			return *this;
		}

		attributes& operator<=(_Ty&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, LESS, EQUAL, SPACE>;

			attr_str_ = sql + std::to_string(t);

			return *this;
		}

		attributes& operator!<(_Ty&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, NOT, LESS, SPACE>;

			attr_str_ = sql + std::to_string(t);

			return *this;
		}

		attributes& operator>(_Ty&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE , GREATER , SPACE>;

			attr_str_ = sql + std::to_string(t);

			return *this;
		}

		attributes& operator>=(_Ty&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, GREATER, EQUAL,SPACE>;

			attr_str_ = sql + std::to_string(t);

			return *this;
		}

		
		attributes& operator!>(_Ty&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, NOT, GREATER, SPACE>;

			attr_str_ = sql + std::to_string(t);

			return *this;
		}

		atrributes& operator|(const attributes& other)
		{
			attr_str_ += OR + other.attr_str_;

			return *this;
		}

		attributes& operator&(const attributes& other)
		{
			attr_str_ += AND + other.attr_str_;

			return *this;
		}

		std::string sql()
		{
			return attr_str_;
		}

	private:
		std::string attr_str_;
	};
} // namespace aquarius