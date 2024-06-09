#pragma once
#include <boost/test/unit_test_suite.hpp>
#include "proto_regist.h"

BOOST_AUTO_TEST_SUITE(client_test)

class ctx_connect_success : public aquarius::context<test_connect_success_request, test_connect_success_response>
{
public:
	ctx_connect_success()
		: aquarius::context<test_connect_success_request, test_connect_success_response>("ctx_connect_success")
	{

	}

public:
	virtual aquarius::error_code handle()
	{

		send_response(0);

		return aquarius::errc::ok;
	}
};

class ctn_connect_success : public aquarius::content<test_connect_success_response>
{
public:
	ctn_connect_success()
		: aquarius::content<test_connect_success_response>("ctn_connect_success")
	{

	}

public:
	virtual aquarius::error_code handle()
	{
		return aquarius::errc::ok;
	}
};

AQUARIUS_CONTEXT_REGIST(test_connect_success_request, ctx_connect_success);
AQUARIUS_CONTEXT_REGIST(test_connect_success_response, ctn_connect_success);


BOOST_AUTO_TEST_CASE(success)
{
	const char* ip_addr = "127.0.0.1";
	aquarius::no_ssl_tcp_server srv(8100, 5);
	std::thread srv_t([&] {srv.run(); });

	std::promise<bool> conn_result{};
	auto future = conn_result.get_future();

	aquarius::no_ssl_tcp_client cli(ip_addr, "8100", [&](auto result)
									{
										conn_result.set_value(result);
									});

	std::thread client_t([&] {cli.run(); });

	std::this_thread::sleep_for(1s);

	auto status = future.wait_for(5s);

	BOOST_CHECK(status == std::future_status::ready);

	BOOST_CHECK(cli.remote_address() == ip_addr);

	BOOST_CHECK(cli.remote_address_u() == aquarius::asio::ip::make_address(ip_addr).to_v4().to_uint());

	BOOST_CHECK(cli.remote_port() == 8100);

	test_connect_success_request req{};
	req.body().age = 1;
	req.body().name = "person";

	cli.send_request(std::move(req), [&](std::shared_ptr<test_connect_success_response> resp_ptr)
					 {
						 BOOST_CHECK(resp_ptr);
					 });

	std::this_thread::sleep_for(3s);

	srv.stop();
	cli.stop();

	srv_t.join();
	client_t.join();
}

BOOST_AUTO_TEST_CASE(failed)
{
	const char* ip_addr = "127.0.0.1";

	std::promise<bool> conn_result{};

	auto future = conn_result.get_future();

	aquarius::no_ssl_tcp_client cli(ip_addr, "8100", [&](auto result)
									{
										conn_result.set_value(result);
									});

	std::thread client_t([&] {cli.run(); });

	auto status = future.wait_for(1s);

	BOOST_CHECK(status != std::future_status::ready);

	auto result = future.get();

	BOOST_CHECK(!result);

	cli.stop();

	client_t.join();
}

BOOST_AUTO_TEST_SUITE_END()