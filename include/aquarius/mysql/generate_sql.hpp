#pragma once
#include <aquarius/mysql/keyword.hpp>
#include <aquarius/type_traits.hpp>
#include <array>
#include <functional>
#include <typeinfo>

#pragma warning(disable : 4100)

namespace aquarius
{
	template <std::string_view const& Keyword, typename _Ty>
	void make_input_sql(std::string& sql, _Ty&& t)
	{
		constexpr static std::string_view table_name = name<_Ty>();

		constexpr auto temp_sql_prev = concat_v<Keyword, SPACE, INTO, SPACE, table_name, SPACE, VALUES, LEFT_BRACKET>;

		sql.append(temp_sql_prev.data());

		aquarius::for_each(std::forward<_Ty>(t),
						   [&](auto&& value)
						   {
							   using type = std::remove_cvref_t<decltype(value)>;
							   if constexpr (std::same_as<type, std::string>)
							   {
								   sql += "'";
								   sql += value;
								   sql += "'";
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

	template <typename _Ty>
	void make_select_sql(std::string& sql)
	{
		constexpr static auto table_name = name<_Ty>();

		constexpr auto temp_sql = concat_v<SELECT, SPACE, ASTERISK, SPACE, FROM, SPACE, table_name, SEPARATOR>;

		sql = std::string(temp_sql.data(), temp_sql.size());
	}

	template <typename _Ty>
	void make_remove_sql(std::string& sql)
	{
		constexpr static auto table_name = name<_Ty>();

		constexpr auto temp_sql_prev = concat_v<REMOVE, SPACE, FROM, SPACE, table_name>;

		sql += temp_sql_prev;

		sql += SEPARATOR;
	}

	template <typename _Ty>
	void make_update_sql(std::string& sql, _Ty&& t)
	{
		constexpr static std::string_view table_name = name<_Ty>();

		constexpr auto temp_sql_prev = concat_v<UPDATE, SPACE, table_name, SPACE>;

		sql.append(temp_sql_prev.data());

		aquarius::for_each(std::forward<_Ty>(t),
						   [&](auto&& value)
						   {
							   sql += concat_v<SET, SPACE, SPACE>;

							   using type = std::remove_cvref_t<decltype(value)>;
							   if constexpr (std::same_as<type, std::string>)
							   {
								   sql += "'";
								   sql += value;
								   sql += "'";
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

} // namespace aquarius