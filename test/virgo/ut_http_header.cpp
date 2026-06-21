#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_header.hpp>

BOOST_AUTO_TEST_SUITE(ut_http_header)

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

BOOST_AUTO_TEST_CASE(http_header_deserialize)
{
    aquarius::flex_buffer buffer;

    aquarius::http_header header{};
    header.content_type("text/plain");
    header.serialize(buffer);

    aquarius::http_header new_header{};
    new_header.deserialize(buffer);

    BOOST_TEST(new_header.content_type() == "text/plain");
}

BOOST_AUTO_TEST_CASE(http_header_deseriialize_failed_syntax)
{
    aquarius::flex_buffer buffer;

    aquarius::http_header header{};
    header.content_type("text/plain");
    header.serialize(buffer);

    buffer.consume(13);

    aquarius::http_header new_header{};
    BOOST_REQUIRE_THROW(new_header.deserialize(buffer), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
