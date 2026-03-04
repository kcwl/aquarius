#include "auth_schedule.h"
#include "proto/login.virgo.h"
#include "server.hpp"
#include <aquarius/module/player_module.hpp>
#include "player.h"

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_GATEWAY_HANDLER(login_request, login_response, ctx_login)
		{
			auto player_ptr = co_await mpc_player_get<player>(session()->uuid());

			if (!player_ptr)
				co_return aquarius::error_code();

			auto result = player_ptr->process_event(verify{});

			if (result == boost::msm::back::HandledEnum::HANDLED_FALSE)
				co_return aquarius::error_code();

			auto auth_res = co_await mpc_auth(this->request()->body().user, request()->body().passwd);

			co_return auth_res ? virgo::http_status::ok : virgo::http_status::unauthorized;
		}
	} // namespace gateway
} // namespace aquarius
