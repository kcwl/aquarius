#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/int_to_string.hpp>
#include <string_view>

BOOST_AUTO_TEST_SUITE(int_to_strings)

using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(check)
{
	constexpr int a = 999;

	constexpr auto temp = aquarius::detail::int_to_string<a>();

	static_assert(temp == "999"sv);

	BOOST_TEST(temp == "999"sv);
}

BOOST_AUTO_TEST_SUITE_END()