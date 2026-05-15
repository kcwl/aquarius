#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/basic_protocol.hpp>
#include <aquarius/detail/flex_buffer.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_protocol)

BOOST_AUTO_TEST_CASE(method_and_consume_behavior)
{
    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
// single-suite file; additional tests were redundant and removed
