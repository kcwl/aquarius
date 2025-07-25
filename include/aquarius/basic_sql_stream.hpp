#pragma once
#include <aquarius/detail/concat.hpp>
#include <aquarius/detail/config.hpp>
#include <aquarius/detail/impl/mysql_keyword.hpp>
#include <aquarius/io_context.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace aquarius
{
	namespace detail
	{
		template <typename _Ty>
		inline consteval std::string_view name()
		{
			using namespace std::string_view_literals;

#ifndef __linux
			constexpr std::string_view name = __FUNCSIG__ ""sv;

			constexpr auto left_bracket = name.find_last_of("<");

			constexpr auto end_bracket = name.find_last_of(">");

			constexpr auto temp_name = name.substr(left_bracket + 1, end_bracket - left_bracket - 1);

			constexpr auto start = name.find_last_of(":");

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
	class basic_sql_stream
	{
	public:
		basic_sql_stream(io_context& context)
			: context_(context)
		{}

	public:
		template <typename U>
		basic_sql_stream& operator<<(U value)
		{
			ss_ << value;
			return *this;
		}

		template <typename T>
		basic_sql_stream insert(T value)
		{
			ss_ << "insert into person values(";

			auto f = [&]<std::size_t... I>(std::index_sequence<I>...)
			{
				constexpr auto size = sizeof...(I);

				((ss_ << boost::pfr::get<I, T>() << (I == size - 1 ? ")" : ",")), ...);
			};

			f(std::make_index_sequence<boost::pfr::tuple_size_v<T>>{});

			return *this;
		}

		template <typename T>
		basic_sql_stream remove(T value)
		{
			constexpr auto struct_name = detail::name<T>();

			constexpr auto delete_sql = concat_v<"delete from "sv, struct_name>;

			ss_ << delete_sql;

			return *this;
		}

		template <typename T>
		basic_sql_stream update(T value)
		{
			constexpr auto struct_name = detail::name<T>();

			constexpr auto update_sql = concat_v<"update "sv, struct_name, " set "sv>;

			ss_ << update_sql;

			boost::pfr::for_each_field_with_name(value,
												 [&](auto name, auto value) { ss_ << name.data() << "=" << value; });

			return *this;
		}

		template <typename T>
		basic_sql_stream select()
		{
			constexpr std::string_view struct_name = detail::name<T>();

			constexpr auto complete_sql = concat_v<SELECT, SPACE, ASTERISK, FROM, SPACE>;

			constexpr auto sql = concat_v<complete_sql, struct_name>;

			ss_ << complete_sql.data();

			return *this;
		}

		template <typename T>
		auto query() -> awaitable<std::vector<T>>
		{
			ss_ << ";";

			co_return co_await context_.sql_query<T>(ss_.str());
		}

		template <typename T>
		auto query_one() -> awaitable<std::optional<T>>
		{
			ss_ << ";";

			auto results = co_await query<T>();

			if (results.empty())
				co_return std::nullopt;

			co_return *results.begin();
		}

		auto execute() -> awaitable<bool>
		{
			ss_ << ";";

			co_return co_await context_.sql_execute(ss_.str());
		}

		std::string sql()
		{
			return ss_.str();
		}

		template <typename Attribute>
		basic_sql_stream& where(const Attribute& attr)
		{
			ss_ << " where " << attr.sql();

			return *this;
		}

		template <typename Attribute>
		basic_sql_stream& _and(const Attribute& attr)
		{
			ss_ << " and " << attr.sql();

			return *this;
		}

		template <typename Attribute>
		basic_sql_stream& _or(const Attribute& attr)
		{
			ss_ << " or " << attr.sql();

			return *this;
		}

		template <bool Increament, const std::string_view&... Column>
		basic_sql_stream& order_by()
		{
			ss_ << " order by ";

			((ss_ << Column << ","), ...);

			ss_.seekg(-1, std::ios_base::cur);

			ss_ << ' ';

			ss_ << (Increament ? " asc" : " desc");

			return *this;
		}

	private:
		Stream ss_;

		io_context& context_;
	};
} // namespace aquarius