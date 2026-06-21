#include <boost/test/unit_test.hpp>
#include <aquarius/resource/ssl_config.hpp>

BOOST_AUTO_TEST_SUITE(ut_ssl_config)

BOOST_AUTO_TEST_CASE(create_and_fields)
{
    auto& cfg = aquarius::create_ssl();

    cfg.crt = "crtdata";
    cfg.key = "keydata";
    cfg.pem = "pemdata";

    BOOST_TEST(cfg.crt == "crtdata");
    BOOST_TEST(cfg.key == "keydata");
    BOOST_TEST(cfg.pem == "pemdata");
}

BOOST_AUTO_TEST_SUITE_END()
