#include <boost/test/unit_test.hpp>
#include <aquarius/virgo/tcp_request.hpp>

BOOST_AUTO_TEST_SUITE(ut_virgo_tcp_request)

BOOST_AUTO_TEST_CASE(nttp_instantiation_compile)
{
    // instantiate template with a router literal and trivial body to ensure NTTP compiles
    struct Body { void serialize(aquarius::flex_buffer&) const {} void deserialize(aquarius::flex_buffer&) {} };

    using Req = aquarius::tcp_request<"10001", Body>;
    (void)sizeof(Req);

    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
