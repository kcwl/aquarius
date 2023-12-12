#pragma once
#include <aquarius/elastic/reflect.hpp>
#include <aquarius/mysql/attributes.hpp>
#include <aquarius/mysql/generate_sql.hpp>
#include <aquarius/mysql/service_pool.hpp>
#include <vector>
#include <aquarius/mysql/to_string.hpp>

using namespace std::string_view_literals;

namespace aquarius
{
	template <typename _Service>
	class basic_sql
	{
	public:
		explicit basic_sql(service_pool<_Service>& pool)
			: pool_(pool)
		{}

		~basic_sql() = default;

	public:
		bool execute()
		{
			return pool_.execute(sql_str_);
		}

		template <typename _Func>
		auto async_execute(_Func&& f)
		{
			return pool_.async_execute(sql_str_, std::forward<_Func>(f));
		}

		template <typename _Ty>
		std::vector<_Ty> query()
		{
			return pool_.template query<_Ty>(sql_str_);
		}

		template <typename _Ty, typename _Func>
		auto async_query(_Func&& f)
		{
			return pool_.async_query(sql_str_, std::forward<_Func>(f));
		}

		std::string sql()
		{
			return sql_str_;
		}

	protected:
		std::string sql_str_;

	private:
		service_pool<_Service>& pool_;
	};

	template <typename _Service>
	class chain_sql : public basic_sql<_Service>
	{
	public:
		chain_sql(service_pool<_Service>& pool)
			: basic_sql<_Service>(pool)
		{}

	public:
		template <typename _Ty>
		chain_sql& remove()
		{
			make_remove_sql<_Ty>(this->sql_str_);

			return *this;
		}

		template <typename _Ty>
		chain_sql& insert(_Ty&& t)
		{
			make_input_sql<INSERT>(this->sql_str_, std::forward<_Ty>(t));

			return *this;
		}

		template <typename _Ty>
		chain_sql& update(_Ty&& t)
		{
			make_update_sql(this->sql_str_, std::forward<_Ty>(t));

			return *this;
		}

		template <typename _Ty>
		chain_sql& replace(_Ty&& t)
		{
			make_input_sql<REPLACE>(this->sql_str_, std::forward<_Ty>(t));

			return *this;
		}

		template <typename _Ty>
		chain_sql& where(_Ty&& f)
		{
			this->sql_str_ += " where" + f.sql();

			return *this;
		}

		template <typename _Ty, _Ty left_value, _Ty right_value>
		chain_sql& where_between()
		{
			constexpr auto sql =
				concat_v<SPACE, WHERE, SPACE, BETWEEN, SPACE, left_value, SPACE, AND, SPACE, right_value, SEPARATOR>;

			if (this->sql_str_.empty())
				return *this;

			if (this->sql_str_.back() == ';')
				this->sql_str_.pop_back();

			this->sql_str_ += sql;

			return *this;
		}

		template <typename _Ty>
		chain_sql& where_is_null()
		{
			constexpr auto sql = concat_v<SPACE, WHERE, SPACE, name<_Ty>(), SPACE, IS_NULL, SEPARATOR>;

			if (this->sql_str_.empty())
				return *this;

			if (this->sql_str_.back() == ';')
				this->sql_str_.pop_back();

			this->sql_str_ += sql;

			return *this;
		}
	};

	template <typename _Service>
	class select_chain : public chain_sql<_Service>
	{
	public:
		explicit select_chain(service_pool<_Service>& pool)
			: chain_sql<_Service>(pool)
		{}

	public:
		template <typename _From, string_literal... args>
		select_chain& select()
		{
			make_select_sql<_From, bind_param<"">::value, bind_param<args>::value...>(this->sql_str_);

			return *this;
		}

		template <typename _From, string_literal... args>
		select_chain& select_distinct()
		{
			make_select_sql<_From, concat_v<DISTINCT,SPACE>, bind_param<args>::value...>(this->sql_str_);

			return *this;
		}

		template <typename _From, std::size_t N, string_literal... args>
		select_chain& select_top()
		{
			make_select_sql<_From, concat_v<TOP, SPACE, to_string<N>::value,SPACE>, bind_param<args>::value...>(this->sql_str_);

			return *this;
		}

		template<std::size_t N>
		select_chain& limit()
		{
			make_cat<concat_v<SPACE, LIMIT, SPACE, to_string<N>::value>>(this->sql_str_);

			return *this;
		}

		template<std::size_t N>
		select_chain& offset()
		{
			make_cat<concat_v<SPACE, OFFSET, SPACE, to_string<N>::value>>(this->sql_str_);

			return *this;
		}

		template<string_literal... args>
		select_chain& order_by()
		{
			make_cat<concat_v<SPACE, ORDER, SPACE, BY, SPACE>, bind_param<args>::value...>(this->sql_str_);

			return *this;
		}

		template <std::size_t ... I>
		select_chain& order_by_index()
		{
			make_cat<concat_v<SPACE, ORDER, SPACE, BY, SPACE>, to_string<I>::value...>(this->sql_str_);

			return *this;
		}

		template<string_literal... args>
		select_chain& group_by()
		{
			make_cat<concat_v<SPACE, GROUP, SPACE, BY,SPACE>, bind_param<args>::value...>(this->sql_str_);

			return *this;
		}

		template<typename _Attr>
		select_chain& having(_Attr&& attr)
		{
			this->sql_str_ += " having";
			this->sql_str_ += attr.sql();
			return *this;
		}
	};

} // namespace aquarius