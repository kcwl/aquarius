#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/make_endpoint.hpp>

BOOST_AUTO_TEST_SUITE(endpoint)

BOOST_AUTO_TEST_CASE(make_endpoint_v4)
{
	auto edp = aquarius::detail::make_v4_endpoint(16);

	BOOST_TEST(edp.port() == 16);

	BOOST_TEST(edp.protocol().family() == AF_INET);
}

BOOST_AUTO_TEST_CASE(make_endpoint_v6)
{
	auto edp = aquarius::detail::make_v6_endpoint(16);

	BOOST_TEST(edp.port() == 16);

	BOOST_TEST(edp.protocol().family() == AF_INET6);
}

BOOST_AUTO_TEST_SUITE_END()