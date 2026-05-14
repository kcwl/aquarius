#include <boost/test/unit_test.hpp>
#include <aquarius/module/mysql_module.hpp>

BOOST_AUTO_TEST_SUITE(ut_module_mysql_module)

BOOST_AUTO_TEST_CASE(init_and_construction_only)
{
#ifdef ENABLE_MYSQL
    // If MYSQL is enabled, we only validate construction and init() returns true
    aquarius::mysql_module m;
    BOOST_TEST(m.init());
#else
    // When MYSQL not enabled, header may be absent; ensure test still compiles
    BOOST_TEST(true);
#endif
}

BOOST_AUTO_TEST_SUITE_END()
