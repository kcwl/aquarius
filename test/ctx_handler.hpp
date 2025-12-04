#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>
#include "test.virgo.h"

using namespace std::string_view_literals;

AQUARIUS_TCP_HANDLER(tcp_login_request, tcp_login_response, ctx_tcp_test)
{
	response().header().uuid = request()->header().uuid;

	response().body().per_resp = request()->body().per_req;

	co_return aquarius::error_code{};
}

AQUARIUS_HTTP_POST_HANDLER(http_login_request, http_login_response, ctx_http_test)
{
	response().header().uuid = request()->header().uuid;

	response().body().per_resp = request()->body().per_req;

	co_return aquarius::virgo::http_status::ok;
}

AQUARIUS_HTTP_POST_HANDLER(new_http_login_request, new_http_login_response, ctx_test_http_hander)
{
	response().body().uuid = request()->body().uuid;
	response().body().per_resp = request()->body().per_req;

	co_return aquarius::virgo::http_status::ok;
}

AQUARIUS_TCP_HANDLER(login_request, login_response, ctx_test_tcp_hander)
{
	if (!request())
	{
		co_return aquarius::error_code{};
	}

	response().header().uuid_ = request()->header().uuid_;

	response().body().per_resp = request()->body().per_req;

	co_return aquarius::error_code{};
}

AQUARIUS_HTTP_GET_HANDLER(http_test_get_request, http_test_get_response, ctx_http_test_get_request)
{
	if (!request())
	{
		co_return aquarius::error_code{};
	}

	response().body().user = request()->body().user;

	response().body().passwd = request()->body().passwd;

	co_return aquarius::error_code{};
}