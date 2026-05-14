#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/detail/make_endpoint.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_detail_make_endpoint)

BOOST_AUTO_TEST_CASE(make_endpoint_ipv4)
{
    auto ep = detail::make_endpoint(12345);

#ifdef USE_IPV6
    BOOST_TEST(ep.protocol().family() == asio::ip::tcp::v6().family());
#else
    BOOST_TEST(ep.protocol().family() == asio::ip::tcp::v4().family());
#endif
    BOOST_TEST(ep.port() == 12345);
}

BOOST_AUTO_TEST_SUITE_END()
