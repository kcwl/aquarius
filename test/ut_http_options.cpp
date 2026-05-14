#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/http/http_options_handler.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/detail/asio.hpp>

using namespace aquarius;

struct mock_session_for_options
{
    mock_session_for_options(asio::io_context&) {}

    auto async_send(flex_buffer&) -> asio::awaitable<error_code>
    {
        co_return error_code{};
    }
};

BOOST_AUTO_TEST_SUITE(ut_http_options_handler)

BOOST_AUTO_TEST_CASE(origin_empty_branch)
{
    asio::io_context io{};

    auto session = std::make_shared<mock_session_for_options>(io);

    auto fut = asio::co_spawn(
        io,
        [session]() -> asio::awaitable<void>
        {
            aquarius::flex_buffer buffer{};
            aquarius::http_header header{};
            header.set_field("Content-Length", "0");
            header.serialize(buffer);

            auto res = co_await mpc_http_options(session, buffer);

            BOOST_TEST(res == error_code{});

            co_return;
        },
        asio::use_future);
}

BOOST_AUTO_TEST_SUITE_END()
