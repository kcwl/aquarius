#include <boost/test/unit_test.hpp>
#include <aquarius/detail/member_pointer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_member_pointer_name)

struct test_mock
{
    int value;
};

BOOST_AUTO_TEST_CASE(get_name)
{
    constexpr auto name = member_pointer_name_v<test_mock, &test_mock::value>;

    static_assert(name == "value");
}

BOOST_AUTO_TEST_SUITE_END()