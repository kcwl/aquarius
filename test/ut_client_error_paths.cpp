#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/basic_client.hpp>

using namespace aquarius;

struct MockSession2
{
    using socket = asio::any_io_executor;
    using resolver = int;

    MockSession2(socket, std::chrono::milliseconds) {}

    asio::awaitable<error_code> async_connect(const std::string&, const std::string&)
    {
        co_return error_code{};
    }

    asio::awaitable<error_code> async_send(flex_buffer&)
    {
        co_return error_code{};
    }

    void close() {}

    asio::awaitable<error_code> query() { co_return error_code{}; }
    asio::awaitable<void> wait(std::size_t) { co_return; }
    asio::awaitable<std::size_t> send_request(std::shared_ptr<void>, std::function<void(flex_buffer&)>, error_code&) { co_return 0; }
};

struct TestClient : public basic_client<MockSession2>
{
    using basic_client::basic_client;
    using basic_client::make_error;
    // expose a public wrapper for testing
    bool call_make_error(const error_code& ec)
    {
        return make_error(const_cast<error_code&>(ec));
    }
    void set_session_ptr(std::shared_ptr<MockSession2> s)
    {
        session_ptr_ = s;
    }
};

BOOST_AUTO_TEST_SUITE(ut_client_error_paths)

BOOST_AUTO_TEST_CASE(make_error_triggers_close)
{
    asio::io_context io;
    TestClient c(io, std::chrono::milliseconds(10));

    bool closed = false;
    c.set_close_func([&](std::shared_ptr<MockSession2>){ closed = true; });

    // manually set a session to avoid coroutine lifetime complexities
    c.set_session_ptr(std::make_shared<MockSession2>(io.get_executor(), c.get_timeout()));

    error_code ec = make_error_code(boost::asio::error::operation_aborted);
    bool res = c.call_make_error(ec);
    BOOST_TEST(res == true);
    BOOST_TEST(closed == true);
}

BOOST_AUTO_TEST_SUITE_END()
