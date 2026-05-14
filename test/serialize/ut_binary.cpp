#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include <vector>
#include <string>

BOOST_AUTO_TEST_SUITE(ut_serialize_binary)

BOOST_AUTO_TEST_CASE(integer_and_string_roundtrip)
{
    aquarius::binary_parse bp;

    // integer roundtrip (unsigned)
    {
        aquarius::flex_buffer buf;
        uint64_t value = 123456789012345ULL;
        bp.to_datas<uint64_t>(value, buf);
        auto out = bp.from_datas<uint64_t>(buf);
        BOOST_TEST(out == value);
    }

    // zig-zag (signed) roundtrip
    {
        aquarius::flex_buffer buf;
        int64_t iv = -12345678;
        bp.to_datas<int64_t>(iv, buf);
        auto out = bp.from_datas<int64_t>(buf);
        BOOST_TEST(out == iv);
    }

    // repeated (vector<int>) roundtrip
    {
        aquarius::flex_buffer buf;
        std::vector<int> vec{1, 2, 3, 200};
        bp.to_datas<std::vector<int>>(vec, buf);
        auto out = bp.from_datas<std::vector<int>>(buf);
        BOOST_TEST(out == vec);
    }

    // string roundtrip
    {
        aquarius::flex_buffer buf;
        std::string s = "hello world";
        bp.to_datas<std::string>(s, buf);
        auto out = bp.from_datas<std::string>(buf);
        BOOST_TEST(out == s);
    }
}

BOOST_AUTO_TEST_SUITE_END()
