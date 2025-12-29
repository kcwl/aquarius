#include "proto/channel.virgo.h"
#include "schedule.hpp"

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_HANDLER(subscribe_tcp_request, subscribe_tcp_response, ctx_subscribe)
		{
			auto& topic = request()->body().topic;

			auto role = std::make_shared<channel_role<session_t>>(session());

			co_await schedule_subscribe(topic, role);

			co_return error_code{};
		}
	} // namespace serivced
} // namespace aquarius