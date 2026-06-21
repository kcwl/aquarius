#include "login.virgo.h"
#include <aquarius.hpp>

AQUARIUS_HANDLER(login_request, login_response, ctx_login_test)
{
	XLOG_INFO() << "login success!";

	co_return error_code{};
}