#pragma once
#include <string>
#include <aquarius/message_body.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <aquarius/context.hpp>
#include <aquarius/detail/router.hpp>


class login_request_body : public aquarius::message_body
{
public:
	virtual std::size_t parse_bytes(aquarius::detail::streambuf& stream) override
	{
		stream << user_name_ << passwd_;

		return 12;
	}

	virtual std::size_t to_bytes(aquarius::detail::streambuf& stream) override
	{
		stream >> user_name_ >> passwd_;
		return 12;
	}

public:
	std::string user_name_;
	std::string passwd_;
};

class login_response_body : public aquarius::message_body
{
public:
	virtual std::size_t parse_bytes(aquarius::detail::streambuf& stream)
	{
		stream >> result_;
		return 4;
	}

	virtual std::size_t to_bytes(aquarius::detail::streambuf& stream)
	{
		stream << result_;
		return 4;
	}

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
		resp_.body()->result_ = 1;

		return true;
	}
};

static aquarius::detail::CtxRegist<login_handler> hanle_context("10001");
static aquarius::detail::MsgRegist<login_request> msg_login("10001");