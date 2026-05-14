#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/http.hpp>
#include <aquarius/virgo/http_method.hpp>
#include <aquarius/virgo/http_status.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_http_parse)

BOOST_AUTO_TEST_CASE(parse_command_line_server_and_client)
{
    error_code ec{};
    http h;

    // server side: method, url, version
    auto [method, url_res, ver] = h.parse_command_line<true>("GET /test HTTP/1.1", ec);
    BOOST_TEST(ec == error_code{});
    BOOST_TEST(method == http_method::get);

    // client side: version status
    auto [v2, status] = h.parse_command_line<false>("HTTP/1.1 200 OK", ec);
    BOOST_TEST(ec == error_code{});
    BOOST_TEST(status == http_status::ok);

    // bad input: wrong parts
    auto res_bad = h.parse_command_line<true>("BADINPUT", ec);
    BOOST_TEST(ec != error_code{});
}

BOOST_AUTO_TEST_CASE(parse_key_behavior)
{
    http h;
    std::string hdr = "source-seq: 123\r\nContent-Length: 0\r\n";
    auto val = h.parse_key(hdr, "source-seq");
    BOOST_TEST(!val.empty());
    BOOST_TEST(val == "123");
}

BOOST_AUTO_TEST_SUITE_END()
