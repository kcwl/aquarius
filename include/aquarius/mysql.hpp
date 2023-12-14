#pragma once
#include <aquarius/mysql/mysql_service.hpp>
#include <aquarius/mysql/service_pool.hpp>
#include <aquarius/mysql/sql.hpp>

namespace
{
	using mysql_pool = aquarius::service_pool<aquarius::mysql_connect>;
}

namespace aquarius
{
	template <typename _Ty, string_literal... args>
	std::vector<_Ty> select(mysql_pool& pool)
	{
		return select_chain(pool).select<_Ty, args...>().query();
	}

	template <typename _Ty, typename _Func>
	auto async_select(mysql_pool& pool, _Func&& f)
	{
		return select_chain(pool).select<_Ty>().async_query(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Attr>
	std::vector<_Ty> select_if(mysql_pool& pool, _Attr&& attr)
	{
		return select_chain(pool).select<_Ty>().where(std::forward<_Attr>(attr)).query<_Ty>();
	}

	template <typename _Ty, typename _Attr, typename _Func>
	auto async_select_if(mysql_pool& pool, _Attr&& attr, _Func&& f)
	{
		return select_chain(pool)
			.select<_Ty>()
			.where(std::forward<_Attr>(attr))
			.async_query<_Ty>(std::forward<_Func>(f));
	}

	template <typename _Ty>
	bool insert(mysql_pool& pool, _Ty&& t)
	{
		return chain_sql(pool).insert(std::forward<_Ty>(t)).execute();
	}

	template <typename _Ty, typename _Func>
	auto async_insert(mysql_pool& pool, _Ty&& t, _Func&& f)
	{
		return chain_sql(pool).insert(std::forward<_Ty>(t)).async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty>
	bool remove(mysql_pool& pool)
	{
		return chain_sql(pool).remove<_Ty>().execute();
	}

	template <typename _Ty, typename _Func>
	auto async_remove(mysql_pool& pool, _Func&& f)
	{
		return chain_sql(pool).remove<_Ty>().async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Attr>
	bool remove_if(mysql_pool& pool, _Attr&& attr)
	{
		return chain_sql(pool).remove<_Ty>().where(std::forward<_Attr>(attr)).execute();
	}

	template <typename _Ty, typename _Attr, typename _Func>
	auto async_remove_if(mysql_pool& pool, _Attr&& attr, _Func&& f)
	{
		return chain_sql(pool)
			.remove<_Ty>()
			.where(std::forward<_Attr>(attr))
			.async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty>
	bool update(mysql_pool& pool, _Ty&& t)
	{
		return chain_sql(pool).update(std::forward<_Ty>(t)).execute();
	}

	template <typename _Ty, typename _Func>
	auto async_update(mysql_pool& pool, _Ty&& t, _Func&& f)
	{
		return chain_sql(pool).update(std::forward<_Ty>(t)).async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Attr>
	bool update_if(mysql_pool& pool, _Ty&& t, _Attr&& attr)
	{
		return chain_sql(pool).update(std::forward<_Ty>(t)).where(std::forward<_Attr>(attr)).execute();
	}

	template <typename _Ty, typename _Attr, typename _Func>
	auto async_update_if(mysql_pool& pool, _Ty&& t, _Attr&& attr, _Func&& f)
	{
		return chain_sql(pool)
			.update(std::forward<_Ty>(t))
			.where(std::forward<_Attr>(attr))
			.async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty>
	bool replace(mysql_pool& pool, _Ty&& t)
	{
		return chain_sql(pool).update(std::forward<_Ty>(t)).execute();
	}

	template <typename _Ty, typename _Func>
	auto async_replace(mysql_pool& pool, _Ty&& t, _Func&& f)
	{
		return chain_sql(pool).update(std::forward<_Ty>(t)).async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Attr>
	bool replace_if(mysql_pool& pool, _Ty&& t, _Attr&& attr)
	{
		return chain_sql(pool).update(std::forward<_Ty>(t)).where(std::forward<_Attr>(attr)).execute();
	}

	template <typename _Ty, typename _Attr, typename _Func>
	auto async_replace_if(mysql_pool& pool, _Ty&& t, _Attr&& attr, _Func&& f)
	{
		return chain_sql(pool)
			.update(std::forward<_Ty>(t))
			.where(std::forward<_Attr>(attr))
			.async_execute(std::forward<_Func>(f));
	}
} // namespace aquarius
