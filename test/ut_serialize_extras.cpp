#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_serialize_extras)

BOOST_AUTO_TEST_CASE(integer_varint_roundtrip_and_overflow)
{
    binary_parse bp;
    aquarius::flex_buffer b;

    // small integer
    bp.to_datas<int>(300, b);
    auto v = bp.from_datas<int>(b);
    BOOST_TEST(v == 300);

    // insufficient buffer for pod_t read should throw
    aquarius::flex_buffer b2;
    bp.to_datas<int>(42, b2);

    // corrupt by consuming a single byte so the buffer is insufficient
    b2.consume(1);

    // now buffer is insufficient and should throw
    BOOST_CHECK_THROW(bp.from_datas<int>(b2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(string_and_vector_and_map_roundtrip)
{
    binary_parse bp;
    aquarius::flex_buffer b;

    std::string s = "hello";
    bp.to_datas(s, b);

    auto s2 = bp.from_datas<std::string>(b);
    BOOST_TEST(s2 == s);

    std::vector<int> v{1,2,3};
    aquarius::flex_buffer b3;
    bp.to_datas(v, b3);
    auto v2 = bp.from_datas<std::vector<int>>(b3);
    BOOST_TEST(v2 == v);

    std::map<int,int> m{{1,10},{2,20}};
    aquarius::flex_buffer b4;
    bp.to_datas(m, b4);
    auto m2 = bp.from_datas<std::map<int,int>>(b4);
    BOOST_TEST(m2 == m);
}

BOOST_AUTO_TEST_SUITE_END()
