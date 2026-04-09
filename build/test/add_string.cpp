#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/tbl/add_string.hpp>
#include <string>

BOOST_AUTO_TEST_SUITE(add_string_tests)

BOOST_AUTO_TEST_CASE(format_string)
{
	std::string str = "hello";
	auto result = aquarius::tbl::add_string_format(str);
	BOOST_TEST(result == "\"hello\"");
}

BOOST_AUTO_TEST_CASE(format_int)
{
	int val = 123;
	auto result = aquarius::tbl::add_string_format(val);
	BOOST_TEST(result == 123);
}

BOOST_AUTO_TEST_SUITE_END()
