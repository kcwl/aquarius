#pragma once
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(servers)

BOOST_AUTO_TEST_CASE(ssl)
{
	aquarius::tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	srv.stop();
	cli.stop();

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_CASE(no_ssl)
{
	aquarius::no_ssl_tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::no_ssl_tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	srv.stop();
	cli.stop();

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_CASE(io_service_pool)
{
	BOOST_CHECK_THROW(aquarius::io_service_pool pool(0), std::runtime_error);

	aquarius::io_service_pool pool(3);

	std::thread t([&] { pool.run(); });

	for (int i = 0; i < 4; ++i)
	{
		pool.get_io_service();
	}

	pool.stop();

	t.join();
}

BOOST_AUTO_TEST_SUITE_END()