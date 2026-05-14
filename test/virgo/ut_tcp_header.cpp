#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/tcp_header.hpp>

BOOST_AUTO_TEST_SUITE(ut_virgo_tcp_header)

using aquarius::flex_buffer;

BOOST_AUTO_TEST_CASE(serialize_deserialize)
{
    aquarius::tcp_header h;
    h.uuid(0xDEADBEEF);
    h.timestamp(1234567890);

    flex_buffer buf{};
    h.serialize(buf);

    aquarius::tcp_header h2;
    h2.deserialize(buf);

    BOOST_TEST(h2.uuid() == 0xDEADBEEF);
    BOOST_TEST(h2.timestamp() == 1234567890);
}

BOOST_AUTO_TEST_SUITE_END()
