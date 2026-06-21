#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/tbl/enter_view.hpp>
#include <aquarius/detail/asio.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_tbl_enter_view)

BOOST_AUTO_TEST_CASE(default_returns_when_no_mysql)
{
    asio::io_context io{};

    auto fut = asio::co_spawn(
        io,
        []() -> asio::awaitable<void>
        {
            // select should return empty vector when ENABLE_MYSQL not set
            auto vec = co_await enter.select<int>("select 1");
            BOOST_TEST(vec.empty());

            auto ins = co_await enter.insert("insert");
            BOOST_TEST(ins == std::size_t(0));

            auto rem = co_await enter.remove("delete");
            BOOST_TEST(rem == std::size_t(0));

            auto up = co_await enter.update("update");
            BOOST_TEST(up == std::size_t(0));

            co_return;
        },
        asio::use_future);

    io.run_for(std::chrono::milliseconds(10));
}

BOOST_AUTO_TEST_SUITE_END()
