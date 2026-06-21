#include <boost/test/unit_test.hpp>
#include <aquarius/detail/concat.hpp>
#include <string_view>
#include <array>

BOOST_AUTO_TEST_SUITE(ut_concat)

using namespace std::string_view_literals;

// Define NTTP-able string_views at namespace scope (external linkage)
inline constexpr std::string_view value1 = "hello "sv;
inline constexpr std::string_view value2 = "world"sv;
inline constexpr std::string_view single = "single"sv;
inline constexpr std::string_view a = "A"sv;
inline constexpr std::string_view empty = ""sv;
inline constexpr std::string_view b = "BC"sv;
inline constexpr std::string_view with_nul = "a\0b"sv; // 'a','\0','b'
inline constexpr std::string_view c = "c"sv;

BOOST_AUTO_TEST_CASE(concats)
{
    constexpr auto result = aquarius::concat_v<value1, value2>;
    BOOST_TEST(result == "hello world"sv);

    constexpr auto single_result = aquarius::concat_v<single>;
    BOOST_TEST(single_result == "single"sv);

    constexpr auto empty_result = aquarius::concat_v<>;
    BOOST_TEST(empty_result.empty());

    constexpr auto multi = aquarius::concat_v<a, empty, b>;
    BOOST_TEST(multi == "ABC"sv);

    constexpr auto nul_concat = aquarius::concat_v<with_nul, c>;
    BOOST_TEST(nul_concat.size() == with_nul.size() + 1);
    BOOST_TEST(nul_concat == "a\0bc"sv);
}

BOOST_AUTO_TEST_CASE(impl_and_arr_inspection)
{
    auto arr = aquarius::detail::concat<value1, value2>::impl();
    constexpr auto expected = "hello world"sv;
    BOOST_TEST(arr.size() == expected.size() + 1);
    for (std::size_t i = 0; i < expected.size(); ++i)
        BOOST_TEST(arr[i] == expected[i]);
    BOOST_TEST(arr[expected.size()] == '\0');

    auto single_arr = aquarius::detail::concat<single>::arr;
    BOOST_TEST(single_arr.size() == single.size() + 1);
    BOOST_TEST(aquarius::concat_v<single> == "single"sv);

    constexpr auto empty_arr = aquarius::detail::concat<>::impl();
    BOOST_TEST(empty_arr.size() == 1u);
    BOOST_TEST(empty_arr[0] == '\0');
    BOOST_TEST((aquarius::concat_v<>).empty());
}

BOOST_AUTO_TEST_SUITE_END()
