#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/basic_tcp_protocol.hpp>
#include <aquarius/virgo/tcp_header.hpp>

BOOST_AUTO_TEST_SUITE(ut_virgo_basic_tcp_protocol)

using aquarius::flex_buffer;

struct SimpleBody
{
    std::string data;

    void serialize(flex_buffer& buf) const
    {
        buf.sputn(data.data(), data.size());
    }

    void deserialize(flex_buffer& buf)
    {
        auto sb = buf.data();
        data.assign((const char*)sb.data(), sb.size());
        buf.consume(sb.size());
    }
};

BOOST_AUTO_TEST_CASE(commit_and_consume_roundtrip)
{
    aquarius::basic_tcp_protocol<true, aquarius::tcp_header, SimpleBody> proto;

    proto.header().uuid(12345);
    proto.header().timestamp(67890);
    proto.body().data = "payload";

    flex_buffer buf{};
    proto.commit(buf);

    aquarius::basic_tcp_protocol<true, aquarius::tcp_header, SimpleBody> copy;
    bool ok = copy.consume(buf);

    BOOST_TEST(ok);
    BOOST_TEST(copy.header().uuid() == 12345u);
    BOOST_TEST(copy.header().timestamp() == 67890);
    BOOST_TEST(copy.body().data == "payload");
}

BOOST_AUTO_TEST_SUITE_END()
