#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_header.hpp>

BOOST_AUTO_TEST_SUITE(ut_virgo_http_header)

using namespace std::string_view_literals;

BOOST_AUTO_TEST_CASE(http_header_fields_and_accessors)
{
    aquarius::http_header h;

    BOOST_TEST(h.find("NonExistent").empty());

    h.set_field("X-Test", "value");
    BOOST_TEST(h.find("X-Test") == "value");

    BOOST_TEST(!h.keep_alive());
    h.keep_alive(true);
    BOOST_TEST(h.keep_alive());

    BOOST_TEST(h.content_type().empty());
    h.content_type("text/plain");
    BOOST_TEST(h.content_type() == "text/plain");

    h.sequence(42);
    BOOST_TEST(h.sequence() == 42u);

    BOOST_TEST(h.content_length() == 0u);
    h.content_length(12345);
    BOOST_TEST(h.content_length() == 12345u);
}

BOOST_AUTO_TEST_SUITE_END()
