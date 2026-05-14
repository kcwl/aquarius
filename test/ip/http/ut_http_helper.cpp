#include <boost/test/unit_test.hpp>
#include <aquarius/ip/http/http_helper.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_ip_http_http_helper)

BOOST_AUTO_TEST_CASE(make_response_command_header_roundtrip)
{
    auto s = make_response_command_header(aquarius::http_status::ok);

    BOOST_TEST(s.find("HTTP/") != std::string::npos);
    BOOST_TEST(s.find("200") != std::string::npos);
    BOOST_TEST(s.find("OK") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
