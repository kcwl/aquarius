#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_status.hpp>

BOOST_AUTO_TEST_SUITE(ut_http_status)

BOOST_AUTO_TEST_CASE(status_string_and_parse)
{
    using namespace std::string_view_literals;

    BOOST_TEST(aquarius::status_to_string(aquarius::http_status::ok) == "OK"sv);

    auto s = aquarius::string_to_status("200");
    BOOST_TEST(s == aquarius::http_status::ok);

    auto s2 = aquarius::string_to_status("9999");
    BOOST_TEST(s2 == aquarius::http_status::internal_server_error);

    auto ec = aquarius::make_error_code(aquarius::http_status::not_found);
    BOOST_TEST(ec.value() == static_cast<int>(aquarius::http_status::not_found));
}

BOOST_AUTO_TEST_CASE(test_unknown_status)
{
    auto result = aquarius::status_to_string(999);

    BOOST_TEST(result == "Unknown");
}

BOOST_AUTO_TEST_CASE(status_category)
{
    auto& category = aquarius::get_http_status_category();

    BOOST_TEST(category.name() == "http status errc");

    BOOST_TEST(category.message(200) == "OK");
}

BOOST_AUTO_TEST_SUITE_END()
