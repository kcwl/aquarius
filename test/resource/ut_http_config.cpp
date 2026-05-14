#include <boost/test/unit_test.hpp>
#include <aquarius/resource/http_config.hpp>

BOOST_AUTO_TEST_SUITE(ut_resource_http_config)

BOOST_AUTO_TEST_CASE(check_origin_method_headers)
{
    auto& cfg = aquarius::create_http();

    cfg.control_allow_origin = "https://a.com,https://b.com";
    cfg.control_allow_methods = "GET,POST";
    cfg.control_allow_headers = "X-Test,Content-Type";

    BOOST_TEST(cfg.check_origin("https://a.com"));
    BOOST_TEST(cfg.check_origin("https://b.com"));
    BOOST_TEST(!cfg.check_origin("https://c.com"));

    BOOST_TEST(cfg.check_method("GET,POST"));
    BOOST_TEST(!cfg.check_method("PUT"));

    BOOST_TEST(cfg.check_headers("X-Test,Content-Type"));
    BOOST_TEST(!cfg.check_headers("X-Other"));
}

BOOST_AUTO_TEST_SUITE_END()
