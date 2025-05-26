#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(details)

BOOST_AUTO_TEST_CASE(io_service_pool)
{
	BOOST_CHECK_THROW(aquarius::detail::io_service_pool pool(0), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(error_code)
{
	aquarius::error_code ec(aquarius::package::pending);

	BOOST_CHECK_EQUAL(ec.value(), static_cast<int>(aquarius::package::pending))
}

BOOST_AUTO_TEST_SUITE_END()