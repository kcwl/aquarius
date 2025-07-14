#include "account_table.h"
#include "error.hpp"
#include "proto/account.proto.hpp"
#include <aquarius.hpp>

namespace db
{
	AQUARIUS_STREAM_CONTEXT(account_check, rpc_query_account)
	{
		auto acc_table = co_await this->sql_engine()
							 .select<account_table>()
							 .where(AQUARIUS_EXPR(account) == this->request()->body().account)
							 .query_one<account_table>();

		if (!acc_table.has_value())
			co_return db_result::invalid_account;

		this->response().body().passwd = (*acc_table).passwd;

		co_return db_result::success;
	}
} // namespace db