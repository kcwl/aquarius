#pragma once
#include <aquarius/mysql/sql_keyword.hpp>
#include <aquarius/type_traits.hpp>
#include <array>
#include <functional>
#include <typeinfo>

#pragma warning(disable : 4100)

namespace aquarius
{
	template <typename _Ty>
	void make_select_sql(std::string& sql)
	{
		constexpr auto temp_sql = concat_v<SELECT, SPACE, ASTERISK, SPACE, FROM, SPACE, name<_Ty>(), SEPARATOR>;

		sql.swap(std::string(temp_sql.data(), temp_sql.size()));
	}

	template <typename _Ty>
	void make_insert_sql(std::string& sql, _Ty&& t)
	{
		constexpr auto temp_sql_prev = concat_v<INSERT, SPACE.INTO, SPACE, name<_Ty>(), SPACE, VALUES, LEFT_BRACKET>;

		sql.append(temp_sql_prev.data());

		for_each(std::forward<_Ty>(t),
				 [&](auto&& value)
				 {
					 using type = std::remove_cvref_t<decltype(value)>;
					 if constexpr (std::same_as<type, std::string>)
					 {
						 sql += "'" + std::to_string(value) + "'";
					 }
					 else
					 {
						 sql += std::to_string(value);
					 }

					 sql += ",";
				 });

		if (sql.back() == ',')
			sql.pop_back();

		sql += RIGHT_BRACKET;
		sql += SEPARATOR;
	}

	template<typename _Ty>
	void make_remove_sql(std::string& sql)
	{
		
		sql += SEPARATOR;
	}

	template <typename _Ty>
	void make_remove_sql(std::string& sql)
	{
		constexpr auto temp_sql_prev = concat_v<REMOVE.SPACE, FROM, SPACE, name<_Ty>()>;

		sql += SEPARATOR;
	}

	namespace detail
	{
		template <typename T>
		std::string create()
		{
			constexpr std::string_view table_name = reflect::rf_name<T>();

			std::string sql = "create table " + std::string(table_name) + " (";

			detail::for_each(T{},
							 [&sql](auto name, auto value, std::size_t&& I)
							 {
								 sql.append("" + std::string(name) + " " +
											detail::mysql_type(detail::indentify<decltype(value)>{}));

								 if (I != reflect::rf_size_v<T> - 1)
									 sql.append(",");
							 });

			return sql;
		}

		template <typename T>
		std::string remove()
		{
			std::string sql = "delete table if exists ";

			constexpr auto table_name = reflect::rf_name<T>();

			sql.append(table_name);

			sql.append(";");

			return sql;
		}

		template <typename Mode, typename T>
		struct generate
		{
			static std::string sql(T&& val, std::string condition)
			{
				return std::string{};
			}
		};

		template <typename T>
		struct generate<replace_mode, T>
		{
			static std::string sql(T const& val, std::string condition)
			{
				constexpr auto table_name = reflect::rf_name<T>();

				std::string sql = "replace into " + std::string(table_name) + " values(";

				detail::for_each(val,
								 [&sql](auto&& value, std::size_t I)
								 {
									 sql += detail::to_string(std::move(value));

									 if (I != reflect::rf_size_v<T> - 1)
										 sql.append(",");
								 });

				sql.append(");");

				return sql;
			}
		};

		template <typename T>
		struct generate<remove_mode, T>
		{
			static std::string sql(const std::string& condition)
			{
				auto table_name = std::string(reflect::rf_name<std::remove_reference_t<T>>());

				std::string sql = "delete from " + std::string(table_name);

				if (!condition.empty())
					sql.append(" where " + condition);

				sql.append(";");

				return sql;
			}
		};

		template <typename T>
		struct generate<select_mode, T>
		{
			static std::string sql(const std::string& condition)
			{
				auto table_name = std::string(reflect::rf_name<std::remove_reference_t<T>>());

				std::string sql = "select * from " + std::string(table_name);

				if (!condition.empty())
					sql.append(" where " + condition);

				sql.append(";");

				return sql;
			}
		};
	} // namespace detail
} // namespace aquarius