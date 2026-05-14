#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_method.hpp>

BOOST_AUTO_TEST_SUITE(ut_virgo_http_request)

using aquarius::flex_buffer;

BOOST_AUTO_TEST_CASE(get_commits_command_header_with_body)
{
    struct Body
    {
        aquarius::http_method m{};
        std::string data;

        void set_method(aquarius::http_method mm) { m = mm; }

        void serialize(flex_buffer& buf) const { buf.sputn(data.data(), data.size()); }

        void deserialize(flex_buffer&)
        {
        }
    };

    using Req = aquarius::http_request<"/test", Body>;

    Req req;
    req.method(aquarius::http_method::get);
    req.body().data = "?q=1";

    flex_buffer buf{};
    req.commit(buf);

    auto sb = buf.data();
    std::string s((const char*)sb.data(), sb.size());

    BOOST_TEST(s.rfind("GET /test", 0) == 0);
    BOOST_TEST(s.find("?q=1") != std::string::npos);
}

BOOST_AUTO_TEST_SUITE_END()
