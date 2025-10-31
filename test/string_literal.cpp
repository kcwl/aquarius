#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/string_literal.hpp>
#include <string_view>

BOOST_AUTO_TEST_SUITE(string_literals)

using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(literals)
{
	constexpr auto str = aquarius::detail::bind_param<"hello">::value;

	static_assert(str == "hello"sv);

	BOOST_TEST(str == "hello"sv);
}

BOOST_AUTO_TEST_SUITE_END()