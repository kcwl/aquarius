#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/concat.hpp>
#include <string_view>

BOOST_AUTO_TEST_SUITE(concat)

using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(concats)
{
	constexpr static auto value1 = "hello "sv;
	constexpr static auto value2 = "world"sv;
	constexpr auto result = aquarius::concat_v<value1, value2>;

	static_assert(result == "hello world"sv);

	BOOST_TEST(result == "hello world"sv);
}

BOOST_AUTO_TEST_SUITE_END()