#pragma once
#include <aquarius.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(server)

BOOST_AUTO_TEST_CASE(io_pools)
{
	BOOST_CHECK_THROW(aquarius::io_service_pool ios(0), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(remote_address)
{
	aquarius::tcp_server<0> srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	aquarius::clear_session();

	srv.stop();
	t.join();
	tc.join();
}

BOOST_AUTO_TEST_SUITE_END()