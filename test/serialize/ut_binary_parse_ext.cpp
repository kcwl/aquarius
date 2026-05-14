#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_serialize_binary_ext)

BOOST_AUTO_TEST_CASE(integer_and_zigzag_roundtrip)
{
    aquarius::flex_buffer b{};
    binary_parse p{};

    uint64_t u = 0x12345;
    p.to_datas<uint64_t>(u, b);
    auto out = p.from_datas<uint64_t>(b);
    BOOST_TEST(out == u);

    int64_t zig = -42;
    aquarius::flex_buffer b2{};
    p.to_datas<int64_t>(zig, b2);
    auto z_out = p.from_datas<int64_t>(b2);
    BOOST_TEST(z_out == zig);
}

BOOST_AUTO_TEST_CASE(string_and_vector_and_map_roundtrip)
{
    aquarius::flex_buffer b{};
    binary_parse p{};

    std::string s = "hello";
    p.to_datas<std::string>(s, b);
    auto s2 = p.from_datas<std::string>(b);
    BOOST_TEST(s2 == s);

    std::vector<int> vec = {1,2,3};
    aquarius::flex_buffer b3{};
    p.to_datas<std::vector<int>>(vec, b3);
    auto vec2 = p.from_datas<std::vector<int>>(b3);
    BOOST_TEST(vec2 == vec);

    std::map<int,std::string> m{{1,"a"},{2,"b"}};
    aquarius::flex_buffer b4{};
    p.to_datas<std::map<int,std::string>>(m, b4);
    auto m2 = p.from_datas<std::map<int,std::string>>(b4);
    BOOST_TEST(m2[1] == "a");
    BOOST_TEST(m2[2] == "b");
}

BOOST_AUTO_TEST_SUITE_END()
