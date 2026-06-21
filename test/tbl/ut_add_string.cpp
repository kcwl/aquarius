#include <boost/test/unit_test.hpp>
#include <aquarius/tbl/add_string.hpp>

BOOST_AUTO_TEST_SUITE(ut_tbl_add_string)

BOOST_AUTO_TEST_CASE(string_and_nonstring)
{
    using aquarius::add_string;

    std::string s = "ok";
    auto quoted = add_string(s);
    BOOST_TEST(quoted == "\"ok\"");

    int x = 5;
    auto nx = add_string(x);
    BOOST_TEST(nx == 5);
}

BOOST_AUTO_TEST_SUITE_END()
