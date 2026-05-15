#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_response.hpp>
#include <aquarius/virgo/http_status.hpp>

BOOST_AUTO_TEST_SUITE(ut_virgo_http_response)

using aquarius::flex_buffer;

struct SimpleBody
{
    std::string data;
    void serialize(flex_buffer& buf) const { buf.sputn(data.data(), data.size()); }
    void deserialize(flex_buffer& buf) { auto sb = buf.data(); data.assign((const char*)sb.data(), sb.size()); buf.consume(sb.size()); }
    std::size_t byte_size() { return data.size() + 1; }
};

BOOST_AUTO_TEST_CASE(commit_header_line)
{
    aquarius::http_response<SimpleBody> resp;
    resp.result(static_cast<int>(aquarius::http_status::ok));
    resp.body().data = "bodytext";

    flex_buffer buf{};
    resp.commit(buf);

    // check header line contains HTTP version and 200 OK
    auto sb = buf.data();
    std::string s((const char*)sb.data(), sb.size());

    BOOST_TEST(s.find("HTTP/") != std::string::npos);
    BOOST_TEST(s.find("200") != std::string::npos);
    BOOST_TEST(s.find("OK") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
