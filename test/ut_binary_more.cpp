#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_binary_more)

BOOST_AUTO_TEST_CASE(zigzag_and_pod_roundtrip)
{
    binary_parse bp;
    aquarius::flex_buffer b;

    // zig-zag encode/decode for negative values
    bp.to_datas<int64_t>(-123, b);
    auto r = bp.from_datas<int64_t>(b);
    BOOST_TEST(r == -123);

    // pod roundtrip
    aquarius::flex_buffer b2;
    bp.to_datas<uint32_t>(0xdeadbeef, b2);
    auto v = bp.from_datas<uint32_t>(b2);
    BOOST_TEST(v == 0xdeadbeefu);
}

BOOST_AUTO_TEST_SUITE_END()
