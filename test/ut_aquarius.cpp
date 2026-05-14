#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_aquarius)

BOOST_AUTO_TEST_CASE(compile_and_basic_binary_roundtrip)
{
    binary_parse bp;
    flex_buffer b;

    bp.to_datas<uint64_t>(0xdeadbeefu, b);

    auto v = bp.from_datas<uint64_t>(b);

    BOOST_CHECK(v == 0xdeadbeefu);
}

BOOST_AUTO_TEST_SUITE_END()
