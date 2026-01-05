#include "proto/login.virgo.h"
#include "player_manager.h"
#include "auth_task.hpp"

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_HANDLER(login_tcp_request, login_tcp_response, ctx_login)
		{
			auto player = aquarius::defer_player(session()->uuid());

			if (!player)
				co_return aquarius::error_code();

			auto result = player->process_event(verify_user{});

			if(result == boost::msm::back::HandledEnum::HANDLED_FALSE)
				co_return aquarius::error_code();

			auto auth_res = aquarius::schedule_module<>("auth");

			co_return auth_res ? aquarius::error_code() : aquarius::error_code();
		}
	}
}

