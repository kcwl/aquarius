#pragma once
#include <aquarius/io_service_pool.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(io_service_pool)

BOOST_AUTO_TEST_CASE(io_service)
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