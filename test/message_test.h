#pragma once
#include "../include/aquarius.hpp"
#include "test_proto.mpr.hpp"

#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(message_process)
using test_request = aquarius::request<xxx::person, 1001>;
using test_response = aquarius::response<xxx::person, 1002>;

class ctx_test_server : public aquarius::xhandle<test_request, test_response>
{
public:
	ctx_test_server()
		: aquarius::xhandle<test_request, test_response>("ctx_test_server")
	{}

public:
	virtual int handle() override
	{
		response_.body().age = 1;
		response_.body().back_money = 2;

		send_response(1);

		return 1;
	}
};

CONTEXT_DEFINE(test_request, ctx_test_server);

class ctx_test_client : public aquarius::client_context<test_response>
{
public:
	ctx_test_client()
		: aquarius::client_context<test_response>("ctx_test_client")
	{}

public:
	virtual int handle() override
	{
		std::cout << "test response recved!\n";

		BOOST_CHECK_EQUAL(request_ptr_->body().age, 1);
		BOOST_CHECK_EQUAL(request_ptr_->body().back_money, 2u);

		return 1;
	}
};

CONTEXT_DEFINE(test_response, ctx_test_client);

BOOST_AUTO_TEST_CASE(process)
{
	aquarius::no_ssl_tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::no_ssl_tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	test_request req{};
	req.body().age = 1;
	req.body().back_money = 2;

	cli.async_write(std::move(req));

	std::this_thread::sleep_for(5s);

	cli.stop();
	tc.join();

	srv.stop();
	t.join();
}

BOOST_AUTO_TEST_SUITE_END()