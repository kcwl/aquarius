#include "client_pool.h"
#include "error.hpp"
#include "make_host_and_port.hpp"
#include "proto/regist.virgo.h"
#include <aquarius.hpp>

namespace aquarius
{
	namespace serviced
	{
		AQUARIUS_SYS_HANDLER(regist_request, regist_response, ctx_regist)
		{
			if (request()->body().group() != "gateway")
			{
				co_await mpc_async_call<&client_pool::shake>(
					request()->body().group(), make_host_and_port(request()->body().host(), request()->body().port()),
					request()->body().name(), request()->body().healthy(), request()->body().weight(),
					request()->body().version());
			}

			co_return errc::success;
		}
	} // namespace serviced
} // namespace aquarius