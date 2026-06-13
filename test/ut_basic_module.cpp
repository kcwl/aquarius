#include <aquarius/basic_module.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;

BOOST_AUTO_TEST_SUITE(ut_basic_module)

struct mock_module
{};

BOOST_AUTO_TEST_CASE(basic_module_name)
{
	module<mock_module> m{};

	BOOST_TEST(m.name() == "mock_module");
}

BOOST_AUTO_TEST_CASE(basic_module_init)
{
	module<mock_module> m{};

	BOOST_TEST(m.init());
}

BOOST_AUTO_TEST_CASE(basic_module_stop)
{
	module<mock_module> m{};

	BOOST_REQUIRE_NO_THROW(m.stop());
}

BOOST_AUTO_TEST_CASE(basic_module_enable)
{
	module<mock_module> m{};

	BOOST_TEST(m.enable());
}

BOOST_AUTO_TEST_CASE(basic_module_timer)
{
	module<mock_module> m{};

	asio::io_context io{};

	auto future = asio::co_spawn(
		io, [m]() mutable -> asio::awaitable<void>
		{ BOOST_REQUIRE_NO_THROW(co_await m.timer(std::chrono::milliseconds(10))); }, asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(basic_module_run)
{
	module<mock_module> m{};

	asio::io_context io{};

	auto future =
		asio::co_spawn(io, [m]() mutable -> asio::awaitable<void> { BOOST_TEST(co_await m.run()); }, asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(module_async_visit)
{
	module<mock_module> m{};

	asio::io_context io{};

	auto f = [](auto* mock) -> asio::awaitable<bool>
	{
		BOOST_TEST(mock);
		co_return true;
	};

	auto future = asio::co_spawn(
		io, [m, func = std::move(f)]() mutable -> asio::awaitable<void>
		{ 
			m.attach(co_await asio::this_coro::executor);

			co_await m.run();

			BOOST_TEST(co_await m.async_visit<bool>(func)); }, asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_SUITE_END()
