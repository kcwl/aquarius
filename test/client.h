#pragma once
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(client_test)

BOOST_AUTO_TEST_CASE(success)
{
	const char* ip_addr = "127.0.0.1";
	aquarius::async_tcp_server srv(8100, 5);
	std::thread srv_t([&] {srv.run(); });

	aquarius::async_tcp_client cli(ip_addr, "8100");

	std::thread client_t([&] {cli.run(); });

	std::this_thread::sleep_for(1s);

	BOOST_CHECK(cli.remote_address() == ip_addr);

	BOOST_CHECK(cli.remote_address_u() == aquarius::asio::ip::make_address(ip_addr).to_v4().to_uint());

	BOOST_CHECK(cli.remote_port() == 8100);

	//test_connect_success_request req{};
	//req.body().age = 1;
	//req.body().name = "person";

	//cli.send_request(std::move(req), [&](std::shared_ptr<test_connect_success_response> resp_ptr)
	//				 {
	//					 BOOST_CHECK(resp_ptr);
	//				 });

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

	aquarius::async_tcp_client cli(ip_addr, "8100");

	std::thread client_t([&] {cli.run(); });

	cli.stop();

	client_t.join();
}

BOOST_AUTO_TEST_SUITE_END()