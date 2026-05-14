#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/basic_protocol.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_protocol)

BOOST_AUTO_TEST_CASE(method_and_consume_behavior)
{
    aquarius::flex_buffer buf{};
    basic_protocol<null_header, null_body*, std::allocator<null_body>> proto;

    proto.method(7);
    BOOST_TEST(proto.method() == 7);

    // null_header::deserialize returns true -> consume should return false
    BOOST_TEST(proto.consume(buf) == false);

    // commit is virtual but default returns error_code{}
    BOOST_TEST(proto.commit(buf) == error_code{});
}

BOOST_AUTO_TEST_SUITE_END()
// single-suite file; additional tests were redundant and removed
