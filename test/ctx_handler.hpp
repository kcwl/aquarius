#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>
#include "test.virgo.h"

using namespace std::string_view_literals;

AQUARIUS_TCP_HANDLER(tcp_login_request, tcp_login_response, ctx_tcp_test)
{
	response().header().uuid = request_ptr_->header().uuid;

	response().body().per_resp = request_ptr_->body().per_req;

	co_return aquarius::error_code{};
}

AQUARIUS_HTTP_HANDLER(http_login_request, http_login_response, ctx_http_test)
{
	response().header().uuid = request_ptr_->header().uuid;

	response().body().per_resp = request_ptr_->body().per_req;

	co_return aquarius::virgo::http_status::ok;
}

AQUARIUS_HTTP_HANDLER(new_http_login_request, new_http_login_response, ctx_test_http_hander)
{
	response().body().per_resp = request_ptr_->body().per_req;

	co_return aquarius::virgo::http_status::ok;
}

AQUARIUS_TCP_HANDLER(login_request, login_response, ctx_test_tcp_hander)
{
	if (!request())
	{
		co_return aquarius::error_code{};
	}

	response().header().uuid_ = request_ptr_->header().uuid_;

	response().body().per_resp = request_ptr_->body().per_req;

	co_return aquarius::error_code{};
}

AQUARIUS_HTTP_HANDLER(http_test_get_request, http_test_get_response, ctx_http_test_get_request)
{
	if (!request())
	{
		co_return aquarius::error_code{};
	}

	response().body().user = request_ptr_->body().user;

	response().body().passwd = request_ptr_->body().passwd;

	co_return aquarius::error_code{};
}