#include "client_pool.h"
#include <proto/regist.virgo.h>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_SYS_HANDLER(publish_request, publish_response, ctx_publish)
		{
			auto& topics = request()->body().topics();

			co_await mpc_async_call<&client_pool::set_topics>(topics);

			co_return gate_op::success;
		}
	} // namespace gateway
} // namespace aquarius
