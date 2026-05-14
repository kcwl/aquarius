#include <boost/test/unit_test.hpp>
#include <aquarius/module/schedule.hpp>
#include <aquarius/module/module_router.hpp>

BOOST_AUTO_TEST_SUITE(ut_module_schedule)

BOOST_AUTO_TEST_CASE(mpc_call_member_traits)
{
    struct TestModule : public aquarius::basic_module<TestModule>
    {
        int foo() { return 5; }
    };

    // register module into router so get_module will find it
    aquarius::module_router::get_mutable_instance().put<TestModule>(0);

    // call member pointer via helper; ensure compilation and basic behavior
    // Note: mpc_call returns an awaitable; here we only check that the member_func_pointer
    // traits compute correct types at compile time by instantiating them.
    using mp = aquarius::member_func_pointer<decltype(&TestModule::foo)>;
    static_assert(std::is_same_v<mp::return_type, int>);
    static_assert(std::is_same_v<mp::class_type, TestModule>);

    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
