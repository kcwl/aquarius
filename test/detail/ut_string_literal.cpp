#include <boost/test/unit_test.hpp>
#include <aquarius/detail/string_literal.hpp>

BOOST_AUTO_TEST_SUITE(ut_detail_string_literal)

using namespace std::string_view_literals;

inline constexpr aquarius::detail::string_literal<3> test_lit("ok");

BOOST_AUTO_TEST_CASE(string_literal_bind_param)
{
    static_assert(aquarius::detail::string_literal<3>::size == 3);

    constexpr auto v = aquarius::detail::bind_param<test_lit>::value;

    static_assert(v == "ok"sv);
}

BOOST_AUTO_TEST_SUITE_END()
