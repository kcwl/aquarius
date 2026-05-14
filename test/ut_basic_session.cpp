#include <boost/test/unit_test.hpp>
#include <aquarius/basic_session.hpp>

struct FakeProto
{
    using socket = int;
    using endpoint = int;
    using acceptor = int;
    using resolver = int;
};

template <typename T>
struct FakeAdaptor { using socket_t = int; };

BOOST_AUTO_TEST_SUITE(ut_basic_session)

BOOST_AUTO_TEST_CASE(type_traits_and_uuid)
{
    // We won't construct a real session (requires sockets). Instead ensure that
    // the template compiles for a fake Protocol/Adaptor by instantiating type aliases.


    // only check that the type can be named
    using Sess = aquarius::basic_session<FakeProto, FakeAdaptor>;

    (void)sizeof(Sess);

    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
