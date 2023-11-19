#pragma once
#include <boost/test/unit_test_suite.hpp>
#include "../include/aquarius.hpp"

BOOST_AUTO_TEST_SUITE(message_process)

struct test_req_body
{
	int a_;
	int b_;
	int c_;
};

struct test_resp_body
{
	int a_;
	int b_;
	int c_;
};

using test_request = aquarius::request<test_req_body, 1001>;
using test_response = aquarius::response<test_resp_body, 1002>;

class ctx_test_server : public aquarius::xhandle<test_request, test_response>
{
public:
	ctx_test_server()
		: aquarius::xhandle<test_request, test_response>("ctx_test_server")
	{}

public:
	virtual int handle() override
	{
		response_.body().a_ = request_ptr_->body().a_ + 1;
		response_.body().b_ = request_ptr_->body().b_ + 1;
		response_.body().c_ = request_ptr_->body().c_ + 1;

		send_response(1);

		return 1;
	}
};

CONTEXT_DEFINE(test_request, ctx_test_server);

class ctx_test_client : public aquarius::client_context<test_response>
{
public:
	ctx_test_client()
		: aquarius::client_context<test_response>("ctx_test_client"){}

public:
	virtual int handle() override
	{
		std::cout << "test response recved!\n";

		BOOST_CHECK_EQUAL(request_ptr_->body().a_, 2);
		BOOST_CHECK_EQUAL(request_ptr_->body().b_, 3);
		BOOST_CHECK_EQUAL(request_ptr_->body().c_, 4);

		return 1;
	}
};


CONTEXT_DEFINE(test_response, ctx_test_client);


BOOST_AUTO_TEST_CASE(process)
{
	aquarius::no_ssl_tcp_server srv(8100, 10);

	std::thread t([&] { srv.run(); });

	aquarius::no_ssl_tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	test_request req{};
	req.body().a_ = 1;
	req.body().b_ = 2;
	req.body().c_ = 3;

	cli.async_write(std::move(req));

	std::this_thread::sleep_for(30s);

	cli.stop();
	tc.join();

	srv.stop();
	t.join();
}

BOOST_AUTO_TEST_SUITE_END()