#include <boost/test/unit_test.hpp>

#include <aquarius/module/module_router.hpp>
#include <aquarius/io_service_pool.hpp>

using namespace aquarius;

struct TestModule : public basic_module<TestModule>
{
};

BOOST_AUTO_TEST_CASE(module_router_put_and_schedule)
{
    module_router router;

    // put should succeed first time
    BOOST_CHECK(router.put<TestModule>(1) == true);

    // second put should fail (duplicate name)
    BOOST_CHECK(router.put<TestModule>(1) == false);

    // schedule should invoke visitor and return value
    int res = router.schedule<TestModule, int>([](TestModule* m) { (void)m; return 123; });
    BOOST_CHECK_EQUAL(res, 123);
}
