#include <boost/test/unit_test.hpp>
#include <aquarius/resource/mysql_config.hpp>

BOOST_AUTO_TEST_SUITE(ut_resource_mysql_config)

BOOST_AUTO_TEST_CASE(create_and_modify)
{
    using aquarius::mysql_config;

    auto& cfg = aquarius::create_mysql();

    cfg.host = "127.0.0.1";
    cfg.user = "u";
    cfg.password = "p";
    cfg.port = 3306;
    cfg.db = "d";
    cfg.pool_size = 4;

    BOOST_TEST(cfg.host == "127.0.0.1");
    BOOST_TEST(cfg.user == "u");
    BOOST_TEST(cfg.port == 3306);
    BOOST_TEST(cfg.pool_size == 4);
}

BOOST_AUTO_TEST_SUITE_END()
