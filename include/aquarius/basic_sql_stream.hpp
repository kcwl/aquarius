#pragma once
#include <sstream>
#include <string>
#include <aquarius/detail/concat.hpp>
#include <aquarius/mysql_pool.hpp>

namespace aquarius
{
	template <typename Stream = std::stringstream>
	class basic_sql_stream
	{
	public:
		basic_sql_stream() = default;

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
			constexpr auto struct_name = name<T>();

			constexpr auto delete_sql = detail::concat_v<"delete from "sv, struct_name>;

			ss_ << delete_sql;

			return *this;
		}

		template <typename T>
		basic_sql_stream update(T value)
		{
			constexpr auto struct_name = name<T>();

			constexpr auto update_sql = detail::concat_v<"update "sv, struct_name, " set "sv>;

			ss_ << update_sql;

			boost::pfr::for_each_field_with_name(value,
												 [&](auto name, auto value) { ss_ << name.data() << "=" << value; });

			return *this;
		}

		template <typename T>
		basic_sql_stream select()
		{
			constexpr auto struct_name = name<T>();

			constexpr std::string_view select_sql = "select * from "sv;

			constexpr auto complete_sql = detail::concat_v<select_sql, struct_name>;

			ss_ << complete_sql.data();

			return *this;
		}

		template <typename T>
		auto async_query() -> awaitable<std::vector<T>>
		{
			ss_ << ";";

			co_return co_await mysql_pool::get_mutable_instance().async_query<T>(ss_.str());
		}

		auto async_execute()
		{
			ss_ << ";";

			co_return co_await mysql_pool::get_mutable_instance().async_execute(ss_.str());
		}

		std::string sql()
		{
			return ss_.str();
		}

		basic_sql_stream& where()
		{
			ss_ << " where ";

			return *this;
		}

		basic_sql_stream& _and()
		{
			ss_ << " and ";

			return *this;
		}

		basic_sql_stream& _or()
		{
			ss_ << " or ";

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
	};
} // namespace aquarius