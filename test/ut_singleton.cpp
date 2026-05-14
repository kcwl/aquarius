#include <boost/test/unit_test.hpp>
#include <aquarius/singleton.hpp>

BOOST_AUTO_TEST_SUITE(ut_singleton)

BOOST_AUTO_TEST_CASE(get_mutable_instance)
{
    struct S { int v = 0; };

    auto& inst = aquarius::singleton<S>::get_mutable_instance();
    inst.v = 7;
    BOOST_TEST(inst.v == 7);
}

BOOST_AUTO_TEST_SUITE_END()
