#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/basic_serialize.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

struct test_serializer : public basic_serialize<binary_parse>
{
    void serialize(flex_buffer& b) override {}
    void deserialize(flex_buffer& b) override {}
};

BOOST_AUTO_TEST_SUITE(ut_basic_serialize)

BOOST_AUTO_TEST_CASE(parse_to_and_from_helpers)
{
    test_serializer s;
    aquarius::flex_buffer b{};

    int v = 42;
    s.parse_to(v, b);
    auto out = s.parse_from<int>(b);
    BOOST_TEST(out == v);
}

BOOST_AUTO_TEST_SUITE_END()
#include <boost/test/unit_test.hpp>
#include <aquarius/basic_serialize.hpp>

BOOST_AUTO_TEST_SUITE(ut_basic_serialize)

BOOST_AUTO_TEST_CASE(compile)
{
    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
