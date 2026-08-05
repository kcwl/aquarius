#include "srvd_bridge.h"
#include <serviced/proto/regist.virgo.h>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_SYS_HANDLER(publish_request, publish_response, ctx_publish)
		{
			auto& topics = request()->body().topics();

			co_await mpc_async_call<&srvd_bridge::set_topics>(topics);

			co_return gate_op::success;
		}
	} // namespace gateway
} // namespace aquarius
