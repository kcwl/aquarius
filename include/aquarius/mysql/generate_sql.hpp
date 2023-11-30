#pragma once
#include <array>
#include <typeinfo>
#include <functional>
#include "sql_type.hpp"
#include <sqlpro/type_traits.hpp>
#include <sqlpro/algorithm.hpp>

#pragma warning(disable:4100)

namespace sqlpro
{
	namespace detail
	{
		template<typename T>
		std::string create()
		{
			constexpr std::string_view table_name = reflect::rf_name<T>();

			std::string sql = "create table " + std::string(table_name) + " (";

			detail::for_each(T{}, [&sql](auto name, auto value, std::size_t&& I)
							 {
								 sql.append("" + std::string(name) + " " + detail::mysql_type(detail::indentify<decltype(value)>{}));

								 if (I != reflect::rf_size_v<T> -1)
									 sql.append(",");
							 });

			return sql;
		}

		template<typename T>
		std::string remove()
		{
			std::string sql = "delete table if exists ";

			constexpr auto table_name = reflect::rf_name<T>();

			sql.append(table_name);

			sql.append(";");

			return sql;
		}

		template<typename Mode, typename T>
		struct generate
		{
			static std::string sql(T&& val, std::string condition)
			{
				return std::string{};
			}
		};

		template<typename T>
		struct generate<replace_mode, T>
		{
			static std::string sql(T const& val, std::string condition)
			{
				constexpr auto table_name = reflect::rf_name<T>();

				std::string sql = "replace into " + std::string(table_name) + " values(";

				detail::for_each(val, [&sql](auto&& value, std::size_t I)
								 {
									 sql += detail::to_string(std::move(value));

									 if (I != reflect::rf_size_v<T> -1)
										 sql.append(",");
								 });

				sql.append(");");

				return sql;
			}
		};

		template<typename T>
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

		template<typename T>
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
	}
}