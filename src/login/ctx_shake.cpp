#include <gateway/proto/shake.virgo.h>
#include <aquarius.hpp>

namespace aquarius
{
	namespace login
	{
		AQUARIUS_SYS_HANDLER(shake_request, shake_response, ctx_shake)
		{
			mpc_generate_topics(response().body().topics());

			co_return error_code{};
		}
	}
}