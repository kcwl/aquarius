#pragma once
#include <aquarius/basic_pool.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/sql/sql_connector.hpp>

namespace aquarius
{
	using sql_pool_type = basic_pool<sql_connector<io_context::executor_type>, io_service_pool>;

	inline std::shared_ptr<sql_pool_type> sql_pool()
	{
		static std::shared_ptr<sql_pool_type> pool_ptr;

		return pool_ptr;
	}

	template <typename IoPool>
	inline void create_sql_pool(IoPool& pool)
	{
		auto pool_ptr = sql_pool();

		if (pool_ptr)
			return;

		pool_ptr = std::make_shared<sql_pool_type>(pool);
	}

} // namespace aquarius