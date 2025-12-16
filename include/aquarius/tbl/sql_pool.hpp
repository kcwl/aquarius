#pragma once
#include <aquarius/basic_pool.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/singleton.hpp>
#include <aquarius/tbl/sql_connector.hpp>

namespace aquarius
{
	class sql_pool_core : public basic_pool<sql_connector, io_service_pool>,
					 public aquarius::singleton<sql_pool_core>
	{};

	inline sql_pool_core& sql_pool()
	{
		return sql_pool_core::get_mutable_instance();
	}

} // namespace aquarius