#include <aquarius/virgo/http_version.hpp>
#include <boost/test/unit_test.hpp>
#include <sstream>

BOOST_AUTO_TEST_SUITE(ut_http_version)

BOOST_AUTO_TEST_CASE(string_conversion_and_stream)
{
	using namespace std::string_view_literals;

	BOOST_TEST(aquarius::version_to_string(aquarius::http_version::http1_1) == "HTTP/1.1"sv);

	auto v = aquarius::string_to_version("HTTP/1.0");
	BOOST_TEST(v.has_value());
	BOOST_TEST(v.value() == aquarius::http_version::http1_0);

	std::stringstream ss;
	ss << aquarius::http_version::http1_1;
	std::string out = ss.str();
	BOOST_TEST(out.find("HTTP/1.1") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(unknown_version)
{
	auto v = aquarius::string_to_version("HTTP/10");

	BOOST_TEST(v.error() == aquarius::http_status::http_version_not_supported);
}

BOOST_AUTO_TEST_SUITE_END()
