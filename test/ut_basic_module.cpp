#include <boost/test/unit_test.hpp>
#include <aquarius/basic_module.hpp>

BOOST_AUTO_TEST_SUITE(ut_basic_module)

BOOST_AUTO_TEST_CASE(name_and_visit)
{
    using namespace aquarius;

    struct MyModule : public basic_module<MyModule> {};

    MyModule m;
    BOOST_TEST(!m.name().empty());

    auto res = m.visit<int>([](MyModule* mm) { return 42; });
    BOOST_TEST(res == 42);
}

BOOST_AUTO_TEST_SUITE_END()
