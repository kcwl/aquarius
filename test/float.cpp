#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/float.hpp>

BOOST_AUTO_TEST_SUITE(floats)

BOOST_AUTO_TEST_CASE(check_equal)
{
	float a = 0.9999999;
	float b = 0.8999999;

	BOOST_TEST(aquarius::check_equal(a, b));
}

BOOST_AUTO_TEST_CASE(check_not_equal)
{
	float a = 0.9999999;
	float b = 1.8999999;

	BOOST_TEST(!aquarius::check_equal(a, b));
}

BOOST_AUTO_TEST_SUITE_END()