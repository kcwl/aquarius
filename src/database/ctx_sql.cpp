#include "proto/execute_sql.virgo.h"
#include <aquarius.hpp>
#include "mysql_module.h"

namespace aquarius
{
	namespace db
	{
		AQUARIUS_HANDLER(execute_sql_request, execute_sql_response, ctx_execute)
		{
			response().body().affected() = static_cast<int32_t>(co_await mpc_async_call<&mysql_module::async_execute>(request()->body().sql()));

			co_return error_code{};
		}

		AQUARIUS_HANDLER(query_sql_request, query_sql_response, ctx_query)
		{
			response().body().results() = co_await mpc_async_call<&mysql_module::async_query_only>(request()->body().sql());

			co_return error_code{};
		}
	}
}