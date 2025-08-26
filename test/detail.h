#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(details)

BOOST_AUTO_TEST_CASE(io_service_pools)
{
	{
		BOOST_CHECK_THROW(aquarius::detail::io_service_pool pool(0), std::runtime_error);
	}
	
	{
		aquarius::detail::io_service_pool pool(1);

		auto& io1 = pool.get_io_service();
		auto& io2 = pool.get_io_service();

		BOOST_CHECK(io1.get_executor() == io2.get_executor());
	}
}

BOOST_AUTO_TEST_SUITE_END()