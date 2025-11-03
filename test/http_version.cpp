#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_version.hpp>

BOOST_AUTO_TEST_SUITE(virgo)

BOOST_AUTO_TEST_CASE(version)
{
	BOOST_TEST(aquarius::virgo::from_version_string(aquarius::virgo::http_version::http1_0) == "HTTP/1.0");
	BOOST_TEST(aquarius::virgo::from_version_string(aquarius::virgo::http_version::http1_1) == "HTTP/1.1");
	BOOST_TEST(aquarius::virgo::from_version_string(aquarius::virgo::http_version::http2) == "HTTP/2");
	BOOST_TEST(aquarius::virgo::from_version_string(aquarius::virgo::http_version::http3) == "HTTP/3");
}

BOOST_AUTO_TEST_CASE(version_streams)
{
	std::cout << aquarius::virgo::http_version::http1_0;

	BOOST_TEST(!std::cout.eof());
}

BOOST_AUTO_TEST_SUITE_END()
