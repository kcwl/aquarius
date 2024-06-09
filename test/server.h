#pragma once
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(servers)

BOOST_AUTO_TEST_CASE(io_service_pool)
{
	BOOST_CHECK_THROW(aquarius::io_service_pool pool(0), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()