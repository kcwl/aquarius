#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/int_to_string.hpp>
#include <aquarius/detail/uuid_generator.hpp>
#include <aquarius/detail/crc.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_detail_misc)

BOOST_AUTO_TEST_CASE(int_to_string_compile_time)
{
    constexpr auto sv = detail::int_to_string<7>();
    BOOST_TEST(sv == "7");

    constexpr auto sv2 = detail::int_to_string<123>();
    BOOST_TEST(sv2 == "123");
}

BOOST_AUTO_TEST_CASE(uuid_and_crc)
{
    detail::uuid_generator g;
    auto id = g();
    BOOST_TEST(id != 0u);

    const char* data = "abc";
    auto c = detail::crc32(data, 3);
    BOOST_TEST(c != 0u);
}

BOOST_AUTO_TEST_CASE(flex_buffer_stream_usage)
{
    asio::streambuf buf;
    std::ostream os(&buf);
    os << "hello";

    std::istream is(&buf);
    std::string s;
    is >> s;
    BOOST_TEST(s == "hello");
}

BOOST_AUTO_TEST_SUITE_END()
