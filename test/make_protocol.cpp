#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/make_protocol.hpp>
#include "test.virgo.h"

BOOST_AUTO_TEST_SUITE(make_proto)

BOOST_AUTO_TEST_CASE(make_tcp_buffers)
{
    login_response response{};

    aquarius::flex_buffer buffer{};
    aquarius::make_tcp_buffer<false>(response, buffer);

    uint32_t value{};
    std::memcpy((void*)&value, buffer.data(), sizeof(value));

    BOOST_TEST(value == buffer.active() - 4);
}

BOOST_AUTO_TEST_CASE(make_http_headlines)
{
    auto headline = aquarius::make_http_headline("HTTP/1.1", "200", "OK");

    BOOST_TEST(headline == "HTTP/1.1 200 OK\r\n");
}

BOOST_AUTO_TEST_SUITE_END()