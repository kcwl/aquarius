#pragma once
#include <aquarius/mysql/mysql_service.hpp>
#include <aquarius/mysql/service_pool.hpp>
#include <aquarius/mysql/sql.hpp>

namespace aquarius
{
	template <typename _Ty, typename _Service>
	std::vector<_Ty> select(service_pool<_Service>& pool)
	{
		return chain_sql<_Service>(pool).select<_Ty>().query();
	}

	template <typename _Ty, typename _Func, typename _Service>
	auto async_select(service_pool<_Service>& pool, _Func&& f)
	{
		return chain_sql<_Service>(pool).select<_Ty>().async_query(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Attr, typename _Service>
	std::vector<_Ty> select_if(service_pool<_Service>& pool, _Attr&& attr)
	{
		return chain_sql<_Service>(pool).select<_Ty>().where(std::forward<_Attr>(attr)).query();
	}

	template <typename _Ty, typename _Attr, typename _Func, typename _Service>
	auto async_select_if(service_pool<_Service>& pool, _Attr&& attr, _Func&& f)
	{
		return chain_sql<_Service>(pool)
			.select<_Ty>()
			.where(std::forward<_Attr>(attr))
			.async_query(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Service>
	bool insert(service_pool<_Service>& pool, _Ty&& t)
	{
		return chain_sql<_Service>(pool).insert(std::forward<_Ty>(t)).execute();
	}

	template <typename _Ty, typename _Func, typename _Service>
	auto async_insert(service_pool<_Service>& pool, _Ty&& t, _Func&& f)
	{
		return chain_sql<_Service>(pool).insert(std::forward<_Ty>(t)).async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Service>
	bool remove(service_pool<_Service>& pool)
	{
		return chain_sql<_Service>(pool).remove<_Ty>().execute();
	}

	template <typename _Ty, typename _Func, typename _Service>
	auto async_remove(service_pool<_Service>& pool, _Func&& f)
	{
		return chain_sql<_Service>(pool).remove<_Ty>().async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Attr, typename _Service>
	bool remove_if(service_pool<_Service>& pool, _Attr&& attr)
	{
		return chain_sql<_Service>(pool).remove<_Ty>().where(std::forward<_Attr>(attr)).execute();
	}

	template <typename _Ty, typename _Attr, typename _Func, typename _Service>
	auto async_remove_if(service_pool<_Service>& pool, _Attr&& attr, _Func&& f)
	{
		return chain_sql<_Service>(pool)
			.remove<_Ty>()
			.where(std::forward<_Attr>(attr))
			.async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Service>
	bool update(service_pool<_Service>& pool, _Ty&& t)
	{
		return chain_sql<_Service>(pool).update(std::forward<_Ty>(t)).execute();
	}

	template <typename _Ty, typename _Func, typename _Service>
	auto async_update(service_pool<_Service>& pool, _Ty&& t, _Func&& f)
	{
		return chain_sql<_Service>(pool).update(std::forward<_Ty>(t)).async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Attr, typename _Service>
	bool update_if(service_pool<_Service>& pool, _Ty&& t, _Attr&& attr)
	{
		return chain_sql<_Service>(pool).update(std::forward<_Ty>(t)).where(std::forward<_Attr>(attr)).execute();
	}

	template <typename _Ty, typename _Attr, typename _Func, typename _Service>
	auto async_update_if(service_pool<_Service>& pool, _Ty&& t, _Attr&& attr, _Func&& f)
	{
		return chain_sql<_Service>(pool)
			.update(std::forward<_Ty>(t))
			.where(std::forward<_Attr>(attr))
			.async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Service>
	bool replace(service_pool<_Service>& pool, _Ty&& t)
	{
		return chain_sql<_Service>(pool).update(std::forward<_Ty>(t)).execute();
	}

	template <typename _Ty, typename _Func, typename _Service>
	auto async_replace(service_pool<_Service>& pool, _Ty&& t, _Func&& f)
	{
		return chain_sql<_Service>(pool).update(std::forward<_Ty>(t)).async_execute(std::forward<_Func>(f));
	}

	template <typename _Ty, typename _Attr, typename _Service>
	bool replace_if(service_pool<_Service>& pool, _Ty&& t, _Attr&& attr)
	{
		return chain_sql<_Service>(pool).update(std::forward<_Ty>(t)).where(std::forward<_Attr>(attr)).execute();
	}

	template <typename _Ty, typename _Attr, typename _Func, typename _Service>
	auto async_replace_if(service_pool<_Service>& pool, _Ty&& t, _Attr&& attr, _Func&& f)
	{
		return chain_sql<_Service>(pool)
			.update(std::forward<_Ty>(t))
			.where(std::forward<_Attr>(attr))
			.async_execute(std::forward<_Func>(f));
	}
} // namespace aquarius