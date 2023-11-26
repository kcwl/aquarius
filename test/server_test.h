#pragma once
#include <aquarius.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(server)

BOOST_AUTO_TEST_CASE(server_connect)
{
	aquarius::no_ssl_tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::no_ssl_tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	BOOST_CHECK(srv.client_count() != 0);

	cli.stop();
	tc.join();

	srv.stop();
	t.join();
}

BOOST_AUTO_TEST_CASE(io_pools)
{
	BOOST_CHECK_THROW(aquarius::io_service_pool ios(0), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(remote_address)
{
	aquarius::no_ssl_tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::no_ssl_tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	BOOST_CHECK(srv.client_count() != 0);

	aquarius::clear_session();

	srv.stop();
	t.join();
	tc.join();
}

BOOST_AUTO_TEST_SUITE_END()