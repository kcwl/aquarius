#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_status.hpp>

BOOST_AUTO_TEST_SUITE(ut_virgo_http_status)

BOOST_AUTO_TEST_CASE(status_string_and_parse)
{
    using namespace std::string_view_literals;

    BOOST_TEST(aquarius::status_to_string(aquarius::http_status::ok) == "OK"sv);

    auto s = aquarius::string_to_status("200");
    BOOST_TEST(s == aquarius::http_status::ok);

    // unknown numeric code -> internal_server_error (fallback)
    auto s2 = aquarius::string_to_status("9999");
    BOOST_TEST(s2 == aquarius::http_status::internal_server_error);

    auto ec = aquarius::make_error_code(aquarius::http_status::not_found);
    BOOST_TEST(ec.value() == static_cast<int>(aquarius::http_status::not_found));
}

BOOST_AUTO_TEST_SUITE_END()
