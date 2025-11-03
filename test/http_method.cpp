#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_method.hpp>

BOOST_AUTO_TEST_SUITE(virgo)

BOOST_AUTO_TEST_CASE(errors)
{
	BOOST_TEST(aquarius::virgo::from_method_string(aquarius::virgo::http_method::get) == "GET");
	BOOST_TEST(aquarius::virgo::from_method_string(aquarius::virgo::http_method::post) == "POST");
	BOOST_TEST(aquarius::virgo::from_method_string(aquarius::virgo::http_method::options) == "OPTIONS");
	BOOST_TEST(aquarius::virgo::from_method_string(aquarius::virgo::http_method::head) == "HEAD");
	BOOST_TEST(aquarius::virgo::from_method_string(aquarius::virgo::http_method::put) == "PUT");
	BOOST_TEST(aquarius::virgo::from_method_string(aquarius::virgo::http_method::delete_) == "DELETE");
	BOOST_TEST(aquarius::virgo::from_method_string(aquarius::virgo::http_method(7)) == "UNKNOWN");
}

BOOST_AUTO_TEST_CASE(method_streams)
{
	std::cout << aquarius::virgo::http_method::get;

	BOOST_TEST(!std::cout.eof());
}

BOOST_AUTO_TEST_SUITE_END()