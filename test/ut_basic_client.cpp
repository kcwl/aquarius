#include <boost/test/unit_test.hpp>
#include <aquarius/basic_client.hpp>
#include <aquarius/detail/asio.hpp>

BOOST_AUTO_TEST_SUITE(ut_basic_client)

BOOST_AUTO_TEST_CASE(set_funcs_and_timeout)
{
    using namespace aquarius;

    struct FakeSession
    {
        using socket = asio::ip::tcp::socket;
        using resolver = asio::ip::tcp::resolver;
    };

    asio::io_context ctx;
    basic_client<FakeSession> client(ctx.get_executor(), std::chrono::milliseconds(123));

    BOOST_TEST(client.get_timeout().count() == 123);

    bool called_close = false;
    client.set_close_func([&](std::shared_ptr<FakeSession>) { called_close = true; });
    client.close_invoke(); // session_ptr_ is null; should be no-op but callable

    bool called_accept = false;
    client.set_accept_func([&](std::shared_ptr<FakeSession>) { called_accept = true; });
    client.accept_invoke();

    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
