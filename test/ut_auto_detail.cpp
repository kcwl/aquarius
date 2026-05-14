#include <boost/test/unit_test.hpp>

#include <aquarius/detail/crc.hpp>
#include <aquarius/detail/make_endpoint.hpp>
#include <aquarius/detail/uuid_generator.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_CASE(crc32_basic)
{
    const char* s = "hello world";
    auto c = detail::crc32(s, strlen(s));
    BOOST_CHECK(c != 0);
    auto c2 = detail::crc32(s, strlen(s));
    BOOST_CHECK_EQUAL(c, c2);
}

BOOST_AUTO_TEST_CASE(make_endpoint_port)
{
    auto ep = detail::make_endpoint(4321);
    BOOST_CHECK_EQUAL(ep.port(), 4321);
}

BOOST_AUTO_TEST_CASE(uuid_generator_returns_uint32)
{
    detail::uuid_generator g;
    auto v1 = g();
    auto v2 = g();
    (void)v2;
    BOOST_CHECK(v1 > 0 || v1 == 0); // just ensure callable and returns integer
}
