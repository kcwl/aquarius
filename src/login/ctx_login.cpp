#include "proto/login.virgo.h"
#include <aquarius.hpp>
#include "auth_module.h"
#include "error.hpp"

namespace aquarius
{
	namespace login
	{
		AQUARIUS_HANDLER(login_request, login_response, ctx_login)
		{
			XLOG_INFO() << "login success!";

			auto& username = request()->body().username();
			auto& passwd = request()->body().password();

			auto is_exist = co_await mpc_async_call<&auth_module::check_password>(username, passwd);

			co_return is_exist != error::success ? http_status::unauthorized : http_status::ok;
		}
	}
}