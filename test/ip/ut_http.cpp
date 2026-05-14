#include <boost/test/unit_test.hpp>
#include <aquarius/ip/http.hpp>
#include <string>

BOOST_AUTO_TEST_SUITE(ut_ip_http)

BOOST_AUTO_TEST_CASE(parse_command_line_and_parse_key)
{
    using namespace std::string_view_literals;

    aquarius::error_code ec{};

    // Server style: METHOD PATH VERSION
    {
        auto header = std::string_view("GET /index.html HTTP/1.1");
        auto res = aquarius::http{}.parse_command_line<true>(header, ec);
        if (ec) {
            BOOST_TEST_MESSAGE("parse_command_line returned error: " << ec.what());
        } else {
            auto method = std::get<0>(res);
            auto version = std::get<2>(res);
            BOOST_TEST(method == aquarius::http_method::get);
            if (version == aquarius::http_version::http1_1 || version == aquarius::http_version::http1_0) {
                BOOST_TEST(true);
            } else {
                BOOST_TEST(false);
            }
        }
    }

    // Client style: VERSION STATUS
    {
        auto header = std::string_view("HTTP/1.1 200");
        ec = aquarius::error_code{};
        auto res = aquarius::http{}.parse_command_line<false>(header, ec);
        if (ec) {
            BOOST_TEST_MESSAGE("parse_command_line returned error: " << ec.what());
        } else {
            auto version = std::get<0>(res);
            auto status = std::get<1>(res);
            if (version == aquarius::http_version::http1_1 || version == aquarius::http_version::http1_0) {
                BOOST_TEST(true);
            } else {
                BOOST_TEST(false);
            }
            if (status == aquarius::http_status::ok || static_cast<int>(status) == 200) {
                BOOST_TEST(true);
            } else {
                BOOST_TEST(false);
            }
        }
    }

    // parse_key
    {
        std::string header_str = "Content-Length: 5\r\nsource-seq: 12\r\n";
        auto val = aquarius::http{}.parse_key(header_str, "source-seq");
        BOOST_TEST(val == "12");
    }
}

BOOST_AUTO_TEST_SUITE_END()
