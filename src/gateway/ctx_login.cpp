#include "auth_schedule.hpp"
#include "player_manager.h"
#include "proto/login.virgo.h"

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_HANDLER(login_tcp_request, login_tcp_response, ctx_login)
		{
			auto player = aquarius::defer_player(session()->uuid());

			if (!player)
				co_return aquarius::error_code();

			auto result = player->process_event(verify{});

			if (result == boost::msm::back::HandledEnum::HANDLED_FALSE)
				co_return aquarius::error_code();

			auto auth_res = co_await mpc_auth<bool>("auth"sv, request()->body().user, request()->body().passwd);

			co_return auth_res ? aquarius::error_code() : aquarius::error_code();
		}
	} // namespace gateway
} // namespace aquarius
