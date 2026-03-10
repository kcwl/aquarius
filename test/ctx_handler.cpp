#include "test.virgo.h"

AQUARIUS_HANDLER(login_request, login_response, ctx_tcp_test)
{
	response().header().uuid(request()->header().uuid());

	response().body().per_resp = request()->body().per_req;

	co_return aquarius::error_code{};
}


AQUARIUS_HANDLER(new_http_login_request, new_http_login_response, ctx_test_http_hander)
{
	response().body().uuid = request()->body().uuid;
	response().body().per_resp = request()->body().per_req;

	co_return aquarius::virgo::http_status::ok;
}

AQUARIUS_HANDLER(http_test_get_request, http_test_get_response, ctx_http_test_get_request)
{
	if (!request())
	{
		co_return aquarius::virgo::http_status::bad_request;
	}

	response().body().user = request()->body().user;

	response().body().passwd = request()->body().passwd;

	co_return aquarius::virgo::http_status::ok;
}