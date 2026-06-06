#include "proto/login.virgo.h"
#include <aquarius.hpp>

namespace aquarius
{
	namespace login
	{
		AQUARIUS_HANDLER(login_tcp_request, login_tcp_response, ctx_login)
		{
			XLOG_INFO() << "login success!";

			co_return error_code{};
		}
	}
}