#include "channel_schedule.h"
#include "error.hpp"
#include "proto/channel.virgo.h"

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_HANDLER(subscribe_tcp_request, subscribe_tcp_response, ctx_subscribe)
		{
			auto& topic = request()->body().topic;

			auto player_ptr = std::make_shared<player>(session()->uuid());

			if (!player_ptr)
				co_return errc::no_player;

			auto result = co_await player_ptr->create_client(session()->get_executor());

			if (!result)
				co_return errc::create_client_error;

			co_await mpc_subscribe(topic, player_ptr);

			co_return errc::success;
		}
	} // namespace serviced
} // namespace aquarius