#include <boost/test/unit_test.hpp>
#include <aquarius/detail/struct_name.hpp>

BOOST_AUTO_TEST_SUITE(ut_detail_struct_name)

BOOST_AUTO_TEST_CASE(struct_name_contents)
{
    using namespace std::string_view_literals;

    auto int_name = aquarius::detail::struct_name<int>();
    BOOST_TEST(int_name.find("int") != std::string_view::npos);

    struct LocalTest {};

    auto local_name = aquarius::detail::struct_name<LocalTest>();
    BOOST_TEST(local_name.find("LocalTest") != std::string_view::npos);
}

BOOST_AUTO_TEST_SUITE_END()
