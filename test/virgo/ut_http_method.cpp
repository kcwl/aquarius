#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_method.hpp>

BOOST_AUTO_TEST_SUITE(ut_http_method)

BOOST_AUTO_TEST_CASE(method_string_and_parse)
{
    using namespace std::string_view_literals;

    BOOST_TEST(aquarius::method_to_string(aquarius::http_method::get) == "GET"sv);

    auto res = aquarius::string_to_method("POST"sv);
    BOOST_TEST(res.has_value());
    BOOST_TEST(res.value() == aquarius::http_method::post);

    auto bad = aquarius::string_to_method("UNKNOWN"sv);
    BOOST_TEST(!bad.has_value());
}

BOOST_AUTO_TEST_CASE(method_stream)
{
    std::stringstream ss{};
    ss << aquarius::http_method::post;

    aquarius::http_method method{};
    ss >> method;

    BOOST_CHECK(method == aquarius::http_method::post);
}

BOOST_AUTO_TEST_SUITE_END()
