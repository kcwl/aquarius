#include "channel_schedule.h"
#include "proto/channel.virgo.h"

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_HANDLER(subscribe_tcp_request, subscribe_tcp_response, ctx_subscribe)
		{
			auto& topic = request()->body().topic;

			auto role = std::make_shared<player>(session()->uuid());

			co_await mpc_subscribe(topic, role);

			co_return error_code{};
		}
	} // namespace serviced
} // namespace aquarius