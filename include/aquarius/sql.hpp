#pragma once
#include <aquarius/detail/string_literal.hpp>
#include <aquarius/sql/and_view.hpp>
#include <aquarius/sql/condition_view.hpp>
#include <aquarius/sql/insert_view.hpp>
#include <aquarius/sql/limit_view.hpp>
#include <aquarius/sql/or_view.hpp>
#include <aquarius/sql/order_view.hpp>
#include <aquarius/sql/remove_view.hpp>
#include <aquarius/sql/select_view.hpp>
#include <aquarius/sql/update_view.hpp>
#include <aquarius/sql/where_view.hpp>
#include <format>
#include <string_view>

namespace aquarius
{
	namespace sql
	{

		template <typename T, auto Member>
		constexpr bool member_find()
		{
			constexpr std::string_view table_name = Member;

			constexpr auto size = boost::pfr::tuple_size_v<std::remove_cvref_t<T>>;

			auto f = []<std::size_t... I>(std::index_sequence<I...>)
			{ return ((table_name.compare(boost::pfr::get_name<I, T>()) == 0) || ...); };

			return f();
		}

		constexpr bool isalpha(char c)
		{
			return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
		}

		constexpr bool isdigit(char c)
		{
			return c >= '0' && c <= '9';
		}

		constexpr bool isalnum(char c)
		{
			return isalpha(c) || isdigit(c);
		}

		constexpr bool isspace(char c)
		{
			return c == ' ';
		}

		template <const std::string_view& Name>
		inline constexpr bool check_table_name()
		{
			if (Name.empty())
				return false;

			constexpr auto iter = Name.begin();

			if (isdigit(*iter))
				return false;

			for (const auto& c : Name)
			{
				if (isalpha(c) || isdigit(c) || c == '_')
					continue;

				return false;
			}

			return true;
		}

		template <const std::string_view& Name>
		constexpr auto trip()
		{
			auto begin_pos = Name.find_first_not_of(' ');

			auto end_pos = Name.find_last_not_of(' ');

			return Name.substr(begin_pos, end_pos - begin_pos + 1);
		}

		template <const std::string_view& Value>
		constexpr auto filter_condition() -> std::string_view
		{
			constexpr auto begin_pos = Value.find_first_not_of(' ');

			if constexpr (Value[begin_pos] == '>')
			{
				if constexpr (Value[begin_pos + 1] == '=')
				{
					return ">="sv;
				}
				else
				{
					return ">"sv;
				}
			}
			else if constexpr (Value[begin_pos] == '<')
			{
				if constexpr (Value[begin_pos + 1] == '=')
				{
					return "<="sv;
				}
				else
				{
					return "<"sv;
				}
			}
			else if constexpr (Value[begin_pos] == '!')
			{
				if constexpr (Value[begin_pos + 1] == '=')
				{
					return "!="sv;
				}
				else
				{
					static_assert(false, "! not support");
				}
			}
			else if constexpr (Value[begin_pos] == '=')
			{
				return "="sv;
			}
			else
			{
				static_assert(false, "syntax error");
			}
		}

		template <auto Value>
		constexpr decltype(auto) parse_condition()
		{
			constexpr std::string_view content = aquarius::detail::bind_param<Value>::value;

			constexpr auto pos = content.find('.');

			constexpr static auto table_name = content.substr(0, pos);

			static_assert(check_table_name<table_name>(), "table name error");

			constexpr auto next_part = content.substr(pos + 1);

			constexpr auto iter = std::find_if(next_part.begin(), next_part.end(),
											   [](auto c) { return !isalnum(c) && c != '_' && !isspace(c); });

			static_assert(iter != next_part.end(), "syntax error");

			constexpr auto len = std::distance(next_part.begin(), iter);

			constexpr static auto member = next_part.substr(0, len);

			constexpr static auto trip_member = trip<member>();

			static_assert(check_table_name<trip_member>(), "member name error");

			constexpr static auto value = next_part.substr(len);

			constexpr auto symbol = filter_condition<value>();

			constexpr static auto cond_value = value.substr(symbol.size());

			constexpr static auto trip_cond_value = trip<cond_value>();

			if constexpr (symbol == aquarius::sql::less::value)
			{
				return aquarius::sql::cond_less<table_name, trip_member, trip_cond_value>();
			}
			else if constexpr (symbol == aquarius::sql::less_equal::value)
			{
				return aquarius::sql::cond_less_equal<table_name, trip_member, trip_cond_value>();
			}
			else if constexpr (symbol == aquarius::sql::greater::value)
			{
				return aquarius::sql::cond_greater<table_name, trip_member, trip_cond_value>();
			}
			else if constexpr (symbol == aquarius::sql::greater_equal::value)
			{
				return aquarius::sql::cond_greater_equal<table_name, trip_member, trip_cond_value>();
			}
			else if constexpr (symbol == aquarius::sql::not_equal::value)
			{
				return aquarius::sql::cond_not_equal<table_name, trip_member, trip_cond_value>();
			}
			else if constexpr (symbol == aquarius::sql::equal::value)
			{
				return aquarius::sql::cond_equal<table_name, trip_member, trip_cond_value>();
			}
			else
			{
				static_assert(false, "symbol error");
			}
		}

		template <std::size_t I, auto Value>
		constexpr auto get_pos_n()
		{
			constexpr std::string_view content = aquarius::detail::bind_param<Value>::value;

			int i = 0;
			std::size_t pos = 0;
			while (i++ < I)
			{
				pos = content.find(',', pos + 1);
			}

			return pos;
		}

		template <const std::string_view& V, const std::string_view& member>
		struct order_param
		{
			constexpr auto operator()() const
			{
				return concat_v<V, member>;
			}
		};

		template <std::size_t I, auto Value>
		constexpr auto get_n()
		{
			constexpr std::string_view content = aquarius::detail::bind_param<Value>::value;

			constexpr auto begin = I == 0 ? 0 : get_pos_n<I, Value>() + 1;

			constexpr auto end = get_pos_n<I + 1, Value>();

			constexpr auto single = content.substr(begin, end - begin);

			constexpr auto pos = single.find('.');

			static_assert(pos != std::string_view::npos, "syntax error");

			constexpr static auto V = single.substr(0, pos);

			constexpr static auto member = single.substr(pos + 1);

			return order_param<V, member>();
		}

		template <auto Value>
		constexpr auto parse_args()
		{
			constexpr static std::string_view content = aquarius::detail::bind_param<Value>::value;

			return aquarius::sql::_tp_condition<content>();
		}
	} // namespace sql
} // namespace aquarius

#define sql_select(content) aquarius::sql::select<content>()
#define sql_insert(content) aquarius::sql::insert(content)
#define sql_update(content) aquarius::sql::update(content)
#define sql_delete(content) aquarius::sql::remove<content>()
#define sql_where(content)                                                                                             \
	aquarius::sql::where(aquarius::sql::parse_condition<aquarius::detail::string_literal(#content)>())
#define sql_and(content)                                                                                               \
	aquarius::sql::and_(aquarius::sql::parse_condition<aquarius::detail::string_literal(#content)>())
#define sql_or(content) aquarius::sql::or_(aquarius::sql::parse_condition<aquarius::detail::string_literal(#content)>())
#define sql_desc(...)                                                                                                  \
	aquarius::sql::desc_odr(aquarius::sql::parse_args<aquarius::detail::string_literal(#__VA_ARGS__)>())
#define sql_asc(...) aquarius::sql::asc_odr(aquarius::sql::parse_args<aquarius::detail::string_literal(#__VA_ARGS__)>())
#define sql_limit(n) aquarius::sql::limit(aquarius::sql::_single_number<n>())