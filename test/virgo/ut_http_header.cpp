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

    {
        std::cerr << "sizeof(http_header)=" << sizeof(aquarius::http_header) << "\n";
        unsigned char* p = reinterpret_cast<unsigned char*>(&h);
        for (size_t i = 0; i < std::min<size_t>(64, sizeof(aquarius::http_header)); ++i)
        {
            char buf[8];
            sprintf(buf, "%02X", p[i]);
            std::cerr << (i < 10 ? " " : "") << i << ":" << buf << " ";
            if ((i + 1) % 8 == 0) std::cerr << "\n";
        }
        std::cerr << std::dec << "\n";
    }
    // Avoid calling `sequence()` before it's set; verify via find()
    {
        auto v = h.find("Seq-Number");
        if (!v.empty()) {
            std::string hex;
            for (unsigned char c : v) {
                char buf[8];
                sprintf(buf, "%02X", c);
                hex += buf;
            }
            BOOST_TEST_MESSAGE("Seq-Number raw: '" << v << "' hex=" << hex << " size=" << v.size());
        } else {
            BOOST_TEST_MESSAGE("Seq-Number empty");
        }
    }
    h.sequence(42);
    BOOST_TEST(h.sequence() == 42u);

    BOOST_TEST(h.content_length() == 0u);
    h.content_length(12345);
    BOOST_TEST(h.content_length() == 12345u);
}

BOOST_AUTO_TEST_SUITE_END()
