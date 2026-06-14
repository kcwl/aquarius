#include <boost/test/unit_test.hpp>
#include <aquarius/detail/struct_name.hpp>

BOOST_AUTO_TEST_SUITE(ut_struct_name)

BOOST_AUTO_TEST_CASE(struct_name_contents)
{
    using namespace std::string_view_literals;

    constexpr auto int_name = aquarius::detail::struct_name<int>();
    BOOST_TEST(int_name.find("int") != std::string_view::npos);

    BOOST_TEST(int_name == "int"sv);

    struct LocalTest {};

    constexpr auto local_name = aquarius::detail::struct_name<LocalTest>();

    BOOST_TEST(local_name == "LocalTest"sv);
}

BOOST_AUTO_TEST_SUITE_END()
