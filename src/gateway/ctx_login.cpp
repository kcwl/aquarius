#include "auth_schedule.h"
#include "gate_error_code.h"
#include "player_schedule.h"
#include "proto/login.virgo.h"
#include "server.hpp"

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_GATEWAY_HANDLER(login_http_request, login_http_response, ctx_login)
		{
			auto player_ptr = co_await mpc_get_player(session()->uuid());

			if (!player_ptr)
				co_return aquarius::error_code();

			auto result = player_ptr->process_event(verify{});

			if (result == boost::msm::back::HandledEnum::HANDLED_FALSE)
				co_return aquarius::error_code();

			auto auth_res = co_await mpc_auth("auth_module"sv, request()->body().user, request()->body().passwd);

			co_return auth_res ? gate_op::success : gate_op::auth_failed;
		}
	} // namespace gateway
} // namespace aquarius
