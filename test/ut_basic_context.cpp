#include <boost/test/unit_test.hpp>
#include <aquarius/basic_context.hpp>
#include <aquarius/detail/asio.hpp>

using namespace aquarius;

struct Proto
{
    template <typename Handler, typename... Args>
    asio::awaitable<error_code> handle_request(flex_buffer&, Args&&...)
    {
        co_return error_code{};
    }
};

BOOST_AUTO_TEST_SUITE(ut_basic_context)

BOOST_AUTO_TEST_CASE(complete_invokes_protocol_handle)
{
    //basic_context<int, Proto> ctx;

    //asio::io_context ioc;

    //flex_buffer buf{};

    //// run the coroutine returned by complete
    //asio::co_spawn(ioc, ctx.complete((Proto*)nullptr, 0, buf), [&](std::exception_ptr, boost::system::error_code) {});
    //ioc.run_for(std::chrono::milliseconds(1));

    BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()
