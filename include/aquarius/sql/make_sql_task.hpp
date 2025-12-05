#pragma once
#include <aquarius/sql/sql_connector.hpp>
#include <aquarius/sql/task.hpp>

namespace aquarius
{
	template<typename T>
	inline auto make_query_task(std::string_view sql)
	{
		return sql_task<sql_tag::query, std::vector<T>>{ sql };
	}

	inline auto make_execute_task(std::string_view sql)
	{
		return sql_task<sql_tag::execute, std::size_t>{ sql };
	}
} // namespace aquarius