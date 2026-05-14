#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_serialize_zigzag)

BOOST_AUTO_TEST_CASE(zigzag_negative_and_positive)
{
    binary_parse bp;
    asio::streambuf b;

    int64_t a = -123456;
    bp.to_datas<int64_t>(a, b);
    auto out = bp.from_datas<int64_t>(b);
    BOOST_TEST(out == a);

    asio::streambuf b2;
    int64_t p = 987654321;
    bp.to_datas<int64_t>(p, b2);
    auto out2 = bp.from_datas<int64_t>(b2);
    BOOST_TEST(out2 == p);
}

BOOST_AUTO_TEST_SUITE_END()
