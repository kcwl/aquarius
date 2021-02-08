#pragma once
#include <string>
#include <aquarius/request.hpp>


class login_request_body : public aquarius::detail::message_body<login_request_body>
{
public:
	virtual std::size_t parse_bytes(aquarius::iostream&& stream, std::size_t length)
	{
		user_name_ = "user";
		passwd_ = "123456";

		return 12;
	}

	void* to_bytes()
	{
		return this;
	}

public:
	std::string user_name_;
	std::string passwd_;
};

class login_response_body : public aquarius::detail::message_body<login_response_body>
{
public:
	virtual std::size_t parse_bytes(aquarius::iostream&&, std::size_t)
	{
		return 0;
	}

	void* to_bytes()
	{
		return this;
	}

public:
	int result_;
};

using login_request = aquarius::net::tcp_request<login_request_body, 10001>;
using login_response = aquarius::net::tcp_request<login_response_body, 10002>;

class handler : public aquarius::net::context<login_request, login_response>
{
public:
	virtual bool handle() override
	{
		resp_.body()->result_ = 1;

		return true;
	}
};

static aquarius::detail::ContextRegister<aquarius::net::basic_context,handler> hanle_context(10001);