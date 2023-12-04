#pragma once
#include <aquarius/mysql/service_pool.hpp>

namespace aquarius
{
	template<typename _Service, typename _Ty>
	void select(service_pool<_Service>& pool, _Ty& t)
	{

	}

	template<typename _Service, typename _Ty>
	void insert(service_pool<_Service>& pool, _Ty&& t)
	{

	}

	template<typename _Service, typename _Ty>
	void remove(service_pool<_Service>& pool, _Ty&& t)
	{

	}

	template<typename _Service, typename _Ty>
	void replace(service_pool<_Service>& pool, _Ty&& t)
	{

	}
}