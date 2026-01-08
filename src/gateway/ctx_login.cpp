#include "auth_schedule.hpp"
#include "proto/login.virgo.h"
#include "player_schedule.hpp"

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_HANDLER(login_tcp_request, login_tcp_response, ctx_login)
		{
			auto player_ptr = co_await mpc_get_player<std::shared_ptr<player>>(session()->uuid());

			if (!player_ptr)
				co_return aquarius::error_code();

			auto result = player_ptr->process_event(verify{});

			if (result == boost::msm::back::HandledEnum::HANDLED_FALSE)
				co_return aquarius::error_code();

			auto auth_res = co_await mpc_auth<bool>("auth"sv, request()->body().user, request()->body().passwd);

			co_return auth_res ? aquarius::error_code() : aquarius::error_code();
		}
	} // namespace gateway
} // namespace aquarius
