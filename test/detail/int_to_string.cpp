#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/int_to_string.hpp>
#include <string_view>

BOOST_AUTO_TEST_SUITE(int_to_string_tests)

using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(to_string_direct)
{
	// direct instantiation of to_string with two chars
	auto sv = aquarius::detail::to_string<'h','i'>();
	std::string_view s = sv;
	BOOST_TEST(s == "hi"sv);
	BOOST_TEST(s.size() == 2);
}

BOOST_AUTO_TEST_CASE(to_string_empty_pack)
{
	// instantiate to_string with an empty pack -> empty string_view
	auto e = aquarius::detail::to_string<>();
	std::string_view s = e;
	BOOST_TEST(s.empty());
	BOOST_TEST(s.size() == 0);
}

BOOST_AUTO_TEST_CASE(to_string_value_member)
{
	// directly access the static constexpr value member
	constexpr auto &arr = aquarius::detail::to_string<'x','y'>::value;
	BOOST_TEST(arr.size() == 2);
	BOOST_TEST(arr[0] == 'x');
	BOOST_TEST(arr[1] == 'y');

	// empty pack value
	constexpr auto &earr = aquarius::detail::to_string<>::value;
	BOOST_TEST(earr.size() == 0);
}

BOOST_AUTO_TEST_CASE(int_single_digits)
{
	BOOST_TEST(aquarius::detail::int_to_string<0>() == "0"sv);
	BOOST_TEST(aquarius::detail::int_to_string<1>() == "1"sv);
	BOOST_TEST(aquarius::detail::int_to_string<5>() == "5"sv);
	BOOST_TEST(aquarius::detail::int_to_string<9>() == "9"sv);
}

BOOST_AUTO_TEST_CASE(int_two_digits)
{
	BOOST_TEST(aquarius::detail::int_to_string<10>() == "10"sv);
	BOOST_TEST(aquarius::detail::int_to_string<11>() == "11"sv);
	BOOST_TEST(aquarius::detail::int_to_string<42>() == "42"sv);
	BOOST_TEST(aquarius::detail::int_to_string<99>() == "99"sv);
}

BOOST_AUTO_TEST_CASE(int_multi_digits)
{
	BOOST_TEST(aquarius::detail::int_to_string<100>() == "100"sv);
	BOOST_TEST(aquarius::detail::int_to_string<1000>() == "1000"sv);
	BOOST_TEST(aquarius::detail::int_to_string<12345>() == "12345"sv);
	BOOST_TEST(aquarius::detail::int_to_string<1234567890>() == "1234567890"sv);
}

BOOST_AUTO_TEST_CASE(misc_cases)
{
	// numbers with repeated digits and zeros
	BOOST_TEST(aquarius::detail::int_to_string<101>() == "101"sv);
	BOOST_TEST(aquarius::detail::int_to_string<1001>() == "1001"sv);
	BOOST_TEST(aquarius::detail::int_to_string<202020>() == "202020"sv);
}

BOOST_AUTO_TEST_SUITE_END()
