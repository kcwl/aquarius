#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius/io_service_pool.hpp>

BOOST_AUTO_TEST_SUITE(io_service_pool)

BOOST_AUTO_TEST_CASE(io_pools)
{ 
	{
		BOOST_CHECK_THROW(aquarius::io_service_pool ios(0), std::runtime_error);
	}
}

BOOST_AUTO_TEST_SUITE_END()