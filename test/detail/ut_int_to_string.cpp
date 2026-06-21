#include <boost/test/unit_test.hpp>
#include <aquarius/detail/int_to_string.hpp>

BOOST_AUTO_TEST_SUITE(ut_int_to_string)

BOOST_AUTO_TEST_CASE(int_to_string_values)
{
    using namespace std::string_view_literals;

    constexpr auto s0 = aquarius::detail::int_to_string<0>();
    BOOST_TEST(s0 == "0"sv);

    constexpr auto s7 = aquarius::detail::int_to_string<7>();
    BOOST_TEST(s7 == "7"sv);

    constexpr auto s123 = aquarius::detail::int_to_string<123>();
    BOOST_TEST(s123 == "123"sv);
}

BOOST_AUTO_TEST_SUITE_END()
