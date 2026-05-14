#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/tcp_serialize.hpp>

BOOST_AUTO_TEST_SUITE(ut_serialize_tcp_serialize)

BOOST_AUTO_TEST_CASE(parse_to_from_helpers)
{
    struct DummyTcpSerialize : public aquarius::tcp_serialize
    {
        void serialize(aquarius::flex_buffer&) override {}
        void deserialize(aquarius::flex_buffer&) override {}
    } ts;

    aquarius::flex_buffer buf;

    uint64_t v = 424242;
    ts.parse_to(v, buf);

    auto out = ts.parse_from<uint64_t>(buf);
    BOOST_TEST(out == v);
}

BOOST_AUTO_TEST_SUITE_END()
