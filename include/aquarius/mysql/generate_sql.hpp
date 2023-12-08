#pragma once
#include <aquarius/mysql/keyword.hpp>
#include <aquarius/type_traits.hpp>
#include <array>
#include <functional>
#include <typeinfo>
#include <aquarius/mysql/string_literal.hpp>

#pragma warning(disable : 4100)

using namespace std::string_view_literals;

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

	template <typename _Ty, std::size_t... I>
	constexpr auto get_tuple(_Ty&& tp, std::index_sequence<I...>)
	{
		return { (std::get<I>(tp), ...) };
	}

	template <typename _Ty>
	constexpr auto get_tuple(_Ty&& tp)
	{
		return get_tuple(std::forward<_Ty>(tp), std::make_index_sequence<std::tuple_size_v<_Ty>()>{});
	}

	//template <typename _From, std::string_view const&... args>
	//void make_select_sql(std::string& sql)
	//{
	//	constexpr static auto table_name = name<_From>();

	//	if constexpr (sizeof...(args) != 0)
	//	{
	//		constexpr std::string_view temp_sql =
	//			concat_v<SELECT, SPACE, (concat_v<args, SPACE, COMMA>, ...), FROM, SPACE, table_name, SEPARATOR>;

	//		constexpr auto pos = temp_sql.find(FROM);

	//		constexpr static std::string_view left = temp_sql.substr(0, pos - 1);

	//		constexpr static std::string_view right = temp_sql.substr(pos);

	//		constexpr auto temp_sql_s = concat_v<left, right>;

	//		sql = std::string(temp_sql_s.data(), temp_sql_s.size());
	//	}
	//	else
	//	{
	//		constexpr auto temp_sql = concat_v<SELECT, SPACE, ASTERISK, SPACE, FROM, SPACE, table_name, SEPARATOR>;

	//		sql = std::string(temp_sql.data(), temp_sql.size());
	//	}
	//}

	template <typename _From, string_literal... args>
	void make_select_sql(std::string& sql)
	{
		constexpr static auto table_name = name<_From>();

		if constexpr (sizeof...(args) != 0)
		{
			constexpr std::string_view temp_sql =
				concat_v<SELECT, SPACE, (concat_v<bind_param<args>::value, SPACE, COMMA>, ...), FROM, SPACE, table_name, SEPARATOR>;

			constexpr auto pos = temp_sql.find(FROM);

			constexpr static std::string_view left = temp_sql.substr(0, pos - 1);

			constexpr static std::string_view right = temp_sql.substr(pos);

			constexpr auto temp_sql_s = concat_v<left, right>;

			sql = std::string(temp_sql_s.data(), temp_sql_s.size());
		}
		else
		{
			constexpr auto temp_sql = concat_v<SELECT, SPACE, ASTERISK, SPACE, FROM, SPACE, table_name, SEPARATOR>;

			sql = std::string(temp_sql.data(), temp_sql.size());
		}
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