#include "error.hpp"
#include "player.h"
#include "proto/regist.virgo.h"
#include <aquarius/module/player_schedule.hpp>

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_HANDLER(regist_tcp_request, regist_tcp_response, ctx_regist)
		{
			auto player_ptr = co_await mpc_player_get<player>("player_module"sv, session()->uuid());

			if (!player_ptr)
				co_return errc::no_player;

			player_ptr->set_addr(request()->body().ip_addr, request()->body().port);

			player_ptr->set_protocol(request()->body().protocol);

			co_return errc::success;
		}
	} // namespace serviced
} // namespace aquarius