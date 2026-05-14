#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/http/http_options_handler.hpp>
#include <aquarius/virgo/http_header.hpp>
#include <aquarius/detail/flex_buffer.hpp>
#include <aquarius/error_code.hpp>
#include <aquarius/detail/asio.hpp>

using namespace aquarius;

struct capture_session
{
    capture_session(asio::io_context&) {}

    auto async_send(flex_buffer& b) -> asio::awaitable<error_code>
    {
        // capture buffer size to ensure response was written
        received_size = b.size();
        co_return error_code{};
    }

    std::size_t received_size{};
};

BOOST_AUTO_TEST_SUITE(ut_ip_http_http_options_handler)

BOOST_AUTO_TEST_CASE(origin_empty_branch)
{
    asio::io_context io{};

    auto session = std::make_shared<capture_session>(io);

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

    io.run_for(std::chrono::milliseconds(10));
}

BOOST_AUTO_TEST_CASE(origin_present_allowed)
{
    asio::io_context io{};

    auto session = std::make_shared<capture_session>(io);

    // configure http to allow the origin/methods/headers
    auto& cfg = create_http();
    cfg.control_allow_origin = "https://a.com";
    cfg.control_allow_methods = "GET,POST";
    cfg.control_allow_headers = "X-Test,Content-Type";
    cfg.control_max_age = "3600";
    cfg.control_allow_credentials = true;

    auto fut = asio::co_spawn(
        io,
        [session]() -> asio::awaitable<void>
        {
            aquarius::flex_buffer buffer{};
            aquarius::http_header header{};
            header.set_field("Origin", "https://a.com");
            header.set_field("Access-Control-Request-Methods", "GET");
            header.set_field("Access-Control-Request-Headers", "X-Test");
            header.set_field("Content-Length", "0");
            header.serialize(buffer);

            auto res = co_await mpc_http_options(session, buffer);

            BOOST_TEST(res == error_code{});
            BOOST_TEST(session->received_size > 0u);

            co_return;
        },
        asio::use_future);

    io.run_for(std::chrono::milliseconds(10));
}

BOOST_AUTO_TEST_SUITE_END()
