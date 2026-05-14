#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/basic_http_protocol.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <aquarius/virgo/http_method.hpp>

BOOST_AUTO_TEST_SUITE(ut_virgo_basic_http_protocol)

using aquarius::flex_buffer;

struct SimpleBody
{
    std::string data;
    void serialize(flex_buffer& buf) const { buf.sputn(data.data(), data.size()); }
    void deserialize(flex_buffer& buf) { auto sb = buf.data(); data.assign((const char*)sb.data(), sb.size()); buf.consume(sb.size()); }
};

BOOST_AUTO_TEST_CASE(post_sets_content_length_and_roundtrip)
{
    aquarius::basic_http_protocol<true, SimpleBody> proto;

    proto.method(aquarius::http_method::post);
    proto.body().data = "hello_world";

    flex_buffer buf{};
    proto.commit(buf);

    aquarius::basic_http_protocol<true, SimpleBody> copy;
    bool ok = copy.consume(buf);

    BOOST_TEST(ok);
    BOOST_TEST(copy.body().data == "hello_world");
}

BOOST_AUTO_TEST_SUITE_END()
