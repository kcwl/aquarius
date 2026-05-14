#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <boost/pfr.hpp>

using namespace aquarius;

struct Small
{
    int a;
    char b;
};

BOOST_AUTO_TEST_SUITE(ut_serialize_reflectable)

BOOST_AUTO_TEST_CASE(reflectable_roundtrip)
{
    binary_parse bp;
    asio::streambuf b;

    Small s{42,'x'};
    bp.to_datas<Small>(s, b);

    auto out = bp.from_datas<Small>(b);

    BOOST_TEST(out.a == s.a);
    BOOST_TEST(out.b == s.b);
}

BOOST_AUTO_TEST_SUITE_END()
