#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/adaptor/raw_adaptor.hpp>
#include <thread>
#include <chrono>

using namespace aquarius;

// Mock adaptor to avoid real network operations in unit tests
namespace aquarius
{
    template<typename T>
    class MockRawAdaptor
    {
        using socket_t = T;

    public:
        enum class Mode { Success, Timeout, Failure };

        MockRawAdaptor(socket_t& socket, Mode mode = Mode::Success, error_code fail_ec = error_code{})
            : socket_(socket), mode_(mode), fail_ec_(fail_ec)
        {}

    public:
        socket_t& get_implement()
        {
            return socket_;
        }

        template <typename Endpoint, typename Dura>
        auto async_connect(const Endpoint&, Dura) -> asio::awaitable<error_code>
        {
            // ensure asynchronous continuation
            co_await asio::post(asio::use_awaitable);

            if (mode_ == Mode::Success)
            {
                co_return error_code{};
            }

            if (mode_ == Mode::Timeout)
            {
                co_return asio::error::timed_out;
            }

            co_return fail_ec_;
        }

        template <typename Dura>
        auto async_handshake(Dura) -> asio::awaitable<error_code>
        {
            co_await asio::post(asio::use_awaitable);
            co_return error_code{};
        }

    private:
        socket_t& socket_;
        Mode mode_;
        error_code fail_ec_;
    };
}

BOOST_AUTO_TEST_CASE(constructor_and_get_implement)
{
    asio::io_context ioc;
    asio::ip::tcp::socket sock(ioc);
    raw_adaptor adaptor(sock);
    BOOST_CHECK(&adaptor.get_implement() == &sock);
}

BOOST_AUTO_TEST_CASE(async_handshake_returns_ok)
{
    asio::io_context ioc;
    asio::ip::tcp::socket sock(ioc);
    raw_adaptor adaptor(sock);

    bool done = false;
    error_code ec;

    asio::co_spawn(ioc,
        [&]() -> asio::awaitable<void> {
            ec = co_await adaptor.async_handshake(std::chrono::milliseconds(10));
            done = true;
            co_return;
        }, asio::detached);

    ioc.run();

    BOOST_CHECK(done);
    BOOST_CHECK(ec == error_code{});
}

BOOST_AUTO_TEST_CASE(async_connect_success)
{
    asio::io_context ioc;

    bool client_done = false;
    error_code client_ec;

    asio::ip::tcp::socket client_sock(ioc);
    MockRawAdaptor<asio::ip::tcp::socket> adaptor(client_sock, MockRawAdaptor<asio::ip::tcp::socket>::Mode::Success);

    asio::co_spawn(ioc,
        [&]() -> asio::awaitable<void> {
            client_ec = co_await adaptor.async_connect(std::vector<asio::ip::tcp::endpoint>{}, std::chrono::seconds(5));
            client_done = true;
            co_return;
        }, asio::detached);

    ioc.run();

    BOOST_CHECK(client_done);
    BOOST_CHECK(client_ec == error_code{});
}

BOOST_AUTO_TEST_CASE(async_connect_timeout)
{
    asio::io_context ioc;

    bool client_done = false;
    error_code client_ec;

    asio::ip::tcp::socket client_sock(ioc);
    MockRawAdaptor<asio::ip::tcp::socket> adaptor(client_sock, MockRawAdaptor<asio::ip::tcp::socket>::Mode::Timeout);

    asio::co_spawn(ioc,
        [&]() -> asio::awaitable<void> {
            client_ec = co_await adaptor.async_connect(std::vector<asio::ip::tcp::endpoint>{}, std::chrono::milliseconds(200));
            client_done = true;
            co_return;
        }, asio::detached);

    ioc.run();

    BOOST_CHECK(client_done);
    BOOST_CHECK(client_ec != error_code{});
}

