#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/detail/mysql_keyword.hpp>

namespace aquarius
{
	template <detail::string_literal sl>
	class attributes
	{
		static constexpr std::string_view sql_begin = concat_v<SPACE, detail::bind_param<sl>::value>;

	public:
		attributes() = default;

		~attributes() = default;

	public:
		template <typename T>
		attributes& operator==(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, EQUAL, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator!=(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, NOT, EQUAL, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator<(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, LESS, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator<=(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, LESS, EQUAL, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator>(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, GREATER, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator>=(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, SPACE, GREATER, EQUAL, SPACE>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		attributes& operator|(const attributes& other)
		{
			attr_str_ += concat_v<SPACE, OR>;
			attr_str_ += other.attr_str_;

			return *this;
		}

		attributes& operator&(const attributes& other)
		{
			attr_str_ += concat_v<SPACE, AND>;

			attr_str_ += other.attr_str_;

			return *this;
		}

		std::string sql() const
		{
			return attr_str_;
		}

	private:
		template <typename T>
		void add_value(T&& t)
		{
			if constexpr (std::same_as<T, std::string>)
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

#define AQUARIUS_EXPR(attr) aquarius::attributes<#attr>()