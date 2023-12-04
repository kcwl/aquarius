#pragma once
#include <aquarius/elastic/reflect.hpp>
#include <aquarius/mysql/attributes.hpp>
#include <aquarius/mysql/generate_sql.hpp>
#include <aquarius/mysql/service_pool.hpp>
#include <vector>

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
		template <typename _Ty>
		bool insert(_Ty&& t)
		{
			make_insert_sql(sql_str_, std::forward<_Ty>(t));

			return execute(sql_str_);
		}

		template <typename _Ty>
		bool replace(_Ty&& t)
		{
			return true;
		}

		bool execute()
		{
			return pool_.execute(sql_str_);
		}

		template <typename _Func>
		auto async_excute(_Func&& f)
		{
			return pool_.async_execute(basic_sql, std::forward<_Func>(f));
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

	private:
		template <typename>
		friend basic_sql& basic_sql::select();

		template <typename _Func>
		basic_sql& where(_Func&& f)
		{
			return *this;
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
		chain_sql& select()
		{
			make_select_sql<_Ty>(sql_str_);

			return *this;
		}

		template <typename _Ty>
		bool remove()
		{
			make_remove_sql(sql_str_);

			return execute();
		}


		template <typename _Ty>
		chain_sql& where(_Ty&& f)
		{
			sql_str_ += f.sql();

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

} // namespace aquarius