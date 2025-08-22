#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/detail/sql_keyword.hpp>
#include <aquarius/concepts/concepts.hpp>

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
			constexpr auto sql = concat_v<sql_begin, EQUAL>;

			attr_str_ += sql.data();
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator!=(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, NOT, EQUAL>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator<(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, LESS>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator<=(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, LESS, EQUAL>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator>(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, GREATER>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template <typename T>
		attributes& operator>=(T&& t)
		{
			constexpr auto sql = concat_v<sql_begin, GREATER, EQUAL>;

			attr_str_ += sql;
			add_value(std::forward<T>(t));

			return *this;
		}

		template<detail::string_literal U>
		attributes& operator|(const attributes<U>& other)
		{
			attr_str_ += concat_v<SPACE, OR>;
			attr_str_ += other.sql();

			return *this;
		}

		template<detail::string_literal U>
		attributes& operator&(const attributes<U>& other)
		{
			attr_str_ += concat_v<SPACE, AND>;

			attr_str_ += other.sql();

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
			if constexpr (is_string<std::decay_t<T>>::value)
			{
				attr_str_ += "\"";
				attr_str_ += t;
				attr_str_ += "\"";
			}
			else
			{
				std::stringstream ss;
				ss << t;
				attr_str_ += ss.str();
			}
		}

	private:
		std::string attr_str_;
	};
} // namespace aquarius

#define AQUARIUS_EXPR(attr) aquarius::attributes<#attr>()