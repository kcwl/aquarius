#include "client_pool.h"
#include <proto/regist.virgo.h>

namespace aquarius
{
	namespace gateway
	{
		AQUARIUS_BASIC_HANDLER(broad_service_status_response, ctx_broad_service_status)
		{
			co_await mpc_async_call<&client_pool::shake>(response().body().host_and_port());

			co_return gate_op::success;
		}
	} // namespace gateway
} // namespace aquarius
