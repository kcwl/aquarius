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
