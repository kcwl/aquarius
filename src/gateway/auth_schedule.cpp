#include "auth_schedule.h"
#include "auth_module.h"
#include <aquarius/module/schedule.hpp>

namespace aquarius
{
	namespace gateway
	{
		auto mpc_auth(std::string_view module_name, const std::string& user, const std::string& passwd)
			-> awaitable<bool>
		{
			co_return co_await mpc::call<bool, auth_module>(module_name, [&](auth_module* op_ptr) -> awaitable<bool>
															{ co_return op_ptr->auth(user, passwd); });
		}
	} // namespace gateway
} // namespace aquarius