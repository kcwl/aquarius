#include <boost/test/unit_test.hpp>
#include <aquarius/detail/concat.hpp>

using namespace std::string_view_literals;

inline constexpr std::string_view lit_a = "a"sv;
inline constexpr std::string_view lit_b = "b"sv;

BOOST_AUTO_TEST_SUITE(ut_detail_concat)

BOOST_AUTO_TEST_CASE(compile_and_basic)
{
    BOOST_TEST((aquarius::concat_v<lit_a, lit_b> == "ab"sv));
}

BOOST_AUTO_TEST_SUITE_END()
