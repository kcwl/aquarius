#include <boost/test/unit_test.hpp>
#include <aquarius/tbl/select_view.hpp>

BOOST_AUTO_TEST_SUITE(ut_tbl_select_view)

BOOST_AUTO_TEST_CASE(default_select_content)
{
    struct Item { int id; };

    auto s = static_cast<std::string>(aquarius::select_v<Item>());

    BOOST_TEST(s.rfind("select", 0) == 0); // starts with select
    BOOST_TEST(s.find("from") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
