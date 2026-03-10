#include "auth_module.h"
#include "table/account.tbl.h"
#include <aquarius/module/module_router.hpp>
#include <aquarius/module/sql_module.hpp>

namespace aquarius
{
	namespace gateway
	{
		auth_module::auth_module(io_context& io, const std::string& name)
			: no_config_module<auth_module>(io, name)
		{}

		auto auth_module::auth(const std::string& username, const std::string& password) -> awaitable<bool>
		{
			std::string sql =
				std::format("select * from account where account={} and passwd={}", username, password);

			auto accounts = co_await mpc_query<account>(sql);

			co_return !accounts.empty();
		}
	} // namespace gateway
} // namespace aquarius

AQUARIUS_MODULE_NAMESPACE(aquarius::gateway, auth_module)