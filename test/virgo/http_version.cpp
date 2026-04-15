#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_version.hpp>

BOOST_AUTO_TEST_SUITE(virgo)

BOOST_AUTO_TEST_CASE(version)
{
	BOOST_TEST(aquarius::version_to_string(aquarius::http_version::http1_0) == "HTTP/1.0");
	BOOST_TEST(aquarius::version_to_string(aquarius::http_version::http1_1) == "HTTP/1.1");
}

BOOST_AUTO_TEST_SUITE_END()
