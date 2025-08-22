#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/sql_keyword.hpp>
#include <boost/pfr.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace aquarius
{
	namespace detail
	{
		template <typename _Ty>
		inline constexpr std::string_view name()
		{
			using namespace std::string_view_literals;

#ifndef __linux
			constexpr std::string_view name = __FUNCSIG__ ""sv;

			constexpr auto left_bracket = name.find_last_of("<");

			constexpr auto end_bracket = name.find_last_of(">");

			constexpr auto temp_name = name.substr(left_bracket + 1, end_bracket - left_bracket - 1);

			constexpr auto start = name.find_last_of(" ");

			if constexpr (start == std::string_view::npos)
			{
				return temp_name;
			}
			else
			{
				return name.substr(start + 1, end_bracket - start - 1);
			}
#else
			constexpr std::string_view name = __PRETTY_FUNCTION__;

			constexpr auto left_bracket = name.find_last_of("[");
			constexpr auto right_bracket = name.find_last_of("]");
			constexpr auto name_in_bracket = name.substr(left_bracket + 1, right_bracket - left_bracket - 1);

			constexpr auto left_equ = name_in_bracket.find_first_of("=");
			constexpr auto right_f = name_in_bracket.find_first_of(";");

			constexpr auto first_name = name_in_bracket.substr(left_equ + 2, right_f - left_equ - 2);

			constexpr auto sp = first_name.find_last_of(":");

			if constexpr (sp == std::string_view::npos)
			{
				return first_name;
			}
			else
			{
				return first_name.substr(sp + 1);
			}
#endif
		}
	} // namespace detail

	template <typename Stream = std::stringstream>
	class sql_stream
	{
	public:
		sql_stream() = default;

	public:
		template <typename U>
		sql_stream& operator<<(U value)
		{
			ss_ << value;
			return *this;
		}

		template <typename T>
		sql_stream& insert(T value)
		{
			constexpr static auto struct_name = detail::name<T>();

			constexpr auto insert_begin =
				concat_v<INSERT, SPACE, INTO, SPACE, struct_name, SPACE, VALUES, LEFT_BRACKET>;

			ss_ << insert_begin.data();

			constexpr auto size = boost::pfr::tuple_size_v<T>;

			auto f = [&]<std::size_t... I>(std::index_sequence<I...>)
			{
				((ss_ << (std::same_as<boost::pfr::tuple_element_t<I, T>, std::string> ? "\"" : "")
					  << boost::pfr::get<I, T>(value)
					  << (std::same_as<boost::pfr::tuple_element_t<I, T>, std::string> ? "\"" : "")
					  << (I == size - 1 ? ")" : ",")),
				 ...);
			};

			f(std::make_index_sequence<size>{});

			return *this;
		}

		template <typename T>
		sql_stream& remove(T /*value*/)
		{
			constexpr static auto struct_name = detail::name<T>();

			constexpr auto delete_sql = concat_v<REMOVE, SPACE, FROM, SPACE, struct_name>;

			ss_ << delete_sql;

			return *this;
		}

		template <typename T>
		sql_stream& update(T value)
		{
			constexpr static auto struct_name = detail::name<T>();

			constexpr auto update_sql = concat_v<UPDATE, SPACE, struct_name, SPACE, SET, SPACE>;

			ss_ << update_sql;

			constexpr auto size = boost::pfr::tuple_size_v<T>;

			boost::pfr::for_each_field_with_name(value, [&](auto name, auto value, std::size_t index)
												 { ss_ << name.data() << "=" << add_string_format(value) << (index != size-1 ? " and " :""); });


			return *this;
		}

		template <typename T>
		sql_stream& select()
		{
			constexpr static auto struct_name = detail::name<T>();

			constexpr static auto complete_sql = concat_v<SELECT, SPACE, ASTERISK, SPACE, FROM, SPACE>;

			constexpr auto sql = concat_v<complete_sql, struct_name>;

			ss_ << sql.data();

			return *this;
		}

		// template <typename T>
		// auto query() -> awaitable<std::vector<T>>
		//{
		//	ss_ << ";";

		//	co_return co_await context_.template sql_query<T>(ss_.str());
		//}

		// template <typename T>
		// auto query_one() -> awaitable<std::optional<T>>
		//{
		//	ss_ << ";";

		//	auto results = co_await query<T>();

		//	if (results.empty())
		//		co_return std::nullopt;

		//	co_return *results.begin();
		//}

		// auto execute() -> awaitable<bool>
		//{
		//	ss_ << ";";

		//	co_return co_await context_.sql_execute(ss_.str());
		//}

		std::string sql()
		{
			return ss_.str();
		}

		template <typename Attribute>
		sql_stream& where(const Attribute& attr)
		{
			ss_ << " where" << attr.sql();

			return *this;
		}

		template <typename Attribute>
		sql_stream& _and(const Attribute& attr)
		{
			ss_ << " and" << attr.sql();

			return *this;
		}

		template <typename Attribute>
		sql_stream& _or(const Attribute& attr)
		{
			ss_ << " or" << attr.sql();

			return *this;
		}

		template <bool Increament, const std::string_view&... Column>
		sql_stream& order_by()
		{
			ss_ << " order by ";

			((ss_ << Column << ","), ...);

			ss_.seekp(-1, std::ios_base::cur);

			ss_ << (Increament ? " asc" : " desc");

			return *this;
		}

	private:
		template<typename T>
		T add_string_format(T value)
		{
			if constexpr (std::same_as<std::remove_cvref_t<T>, std::string>)
			{
				value = "\"" + value + "\"";
			}

			return value;
		}

	private:
		Stream ss_;
	};
} // namespace aquarius