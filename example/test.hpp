#pragma once
#include <string>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/context.hpp>
#include <aquarius/router.hpp>


struct login_request_body
{
	std::string user_name_;
	std::string passwd_;
};

class login_response_body
{
public:
	int result_;
};

using login_request = aquarius::request<login_request_body, 10001>;
using login_response = aquarius::response<login_response_body, 10002>;

class login_handler : public aquarius::handler<login_request, login_response>
{
public:
	virtual bool handle() override
	{
		resp_.body().result_ = 1;

		return true;
	}
};

static aquarius::ctx_regist<login_handler> hanle_context("10001");
static aquarius::msg_regist<login_request> msg_login("10001");