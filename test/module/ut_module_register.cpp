#include <boost/test/unit_test.hpp>
#include <aquarius/module/module_register.hpp>
BOOST_AUTO_TEST_SUITE(ut_module_module_register)

BOOST_AUTO_TEST_CASE(put_and_duplicate)
{
    aquarius::module_router& router = aquarius::module_router::get_mutable_instance();

    // define a local module type
    struct LocalModule : public aquarius::basic_module<LocalModule>
    {};

    // first put should succeed
    bool r1 = router.put<LocalModule>(0);
    BOOST_TEST(r1);

    // duplicate put should return false
    bool r2 = router.put<LocalModule>(0);
    BOOST_TEST(!r2);
}

BOOST_AUTO_TEST_SUITE_END()
