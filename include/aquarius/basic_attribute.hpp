#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/mysql_keyword.hpp>
#include <aquarius/detail/string_literal.hpp>

namespace aquarius
{
	template <detail::string_literal sl>
	class attributes
	{
		static constexpr std::string_view sql_begin = detail::concat_v<SPACE, detail::bind_param<sl>::value>;

	public:
		attributes() = default;

		~attributes() = default;

	public:
		template <typename _Ty>
		attributes& operator==(_Ty&& t)
		{
			constexpr auto sql = detail::concat_v<sql_begin, SPACE, EQUAL, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<_Ty>(t));

			return *this;
		}

		template <typename _Ty>
		attributes& operator!=(_Ty&& t)
		{
			constexpr auto sql = detail::concat_v<sql_begin, SPACE, NOT, EQUAL, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<_Ty>(t));

			return *this;
		}

		template <typename _Ty>
		attributes& operator<(_Ty&& t)
		{
			constexpr auto sql = detail::concat_v<sql_begin, SPACE, LESS, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<_Ty>(t));

			return *this;
		}

		template <typename _Ty>
		attributes& operator<=(_Ty&& t)
		{
			constexpr auto sql = detail::concat_v<sql_begin, SPACE, LESS, EQUAL, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<_Ty>(t));

			return *this;
		}

		template <typename _Ty>
		attributes& operator>(_Ty&& t)
		{
			constexpr auto sql = detail::concat_v<sql_begin, SPACE, GREATER, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<_Ty>(t));

			return *this;
		}

		template <typename _Ty>
		attributes& operator>=(_Ty&& t)
		{
			constexpr auto sql = detail::concat_v<sql_begin, SPACE, GREATER, EQUAL, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<_Ty>(t));

			return *this;
		}

		attributes& operator|(const attributes& other)
		{
			attr_str_ += detail::concat_v<SPACE, OR>;
			attr_str_ += other.attr_str_;

			return *this;
		}

		attributes& operator&(const attributes& other)
		{
			attr_str_ += detail::concat_v<SPACE, AND>;

			attr_str_ += other.attr_str_;

			return *this;
		}

		std::string sql()
		{
			return attr_str_;
		}

	private:
		template <typename _Ty>
		void add_value(_Ty&& t)
		{
			if constexpr (std::same_as<_Ty, std::string>)
			{
				attr_str_.append("'");
				attr_str_.append(t);
				attr_str_.append("'");
			}
			else
			{
				attr_str_ += std::to_string(t);
			}
		}

	private:
		std::string attr_str_;
	};
} // namespace aquarius