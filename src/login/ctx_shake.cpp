#include <gateway/proto/shake.virgo.h>
#include <aquarius.hpp>

namespace aquarius
{
	namespace login
	{
		AQUARIUS_HANDLER(shake_tcp_request, shake_tcp_response, ctx_shake)
		{
			mpc_generate_topics(response().body().topics());

			co_return error_code{};
		}
	}
}