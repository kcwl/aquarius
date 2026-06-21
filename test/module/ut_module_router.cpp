#include <aquarius/module/module_router.hpp>
#include <boost/test/unit_test.hpp>

using namespace aquarius;
using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(ut_module_router)

BOOST_AUTO_TEST_CASE(ctor)
{
	BOOST_TEST(true);
}

struct mock_module : public module<mock_module>
{};

BOOST_AUTO_TEST_CASE(put_module)
{
	module_router r{};

	BOOST_TEST(r.put<mock_module>());
}

BOOST_AUTO_TEST_CASE(run)
{
	module_router r{};

	io_service_pool pool(2);

	asio::io_context io{};

	auto future = asio::co_spawn(
		io,
		[&]() -> asio::awaitable<void>
		{
			BOOST_REQUIRE_NO_THROW(r.run(pool));
			co_return;
		},
		asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_CASE(schedule_success)
{
	module_router r{};

	r.put<mock_module>();

	io_service_pool pool(1);

	auto future = asio::co_spawn(
		pool.get_io_service(),
		[&]() -> asio::awaitable<void>
		{
			co_await r.run(pool);

			auto result =
				co_await r.async_schedule<mock_module, bool>([](auto) -> asio::awaitable<bool> { co_return true; });

			BOOST_TEST(result);
		},
		asio::use_future);

	std::thread t([&] { pool.run(); });

	future.get();

	pool.stop();

	t.join();
}

BOOST_AUTO_TEST_CASE(schedule_failed)
{
	module_router r{};

	io_service_pool pool(1);

	auto future = asio::co_spawn(
		pool.get_io_service(),
		[&]() -> asio::awaitable<void>
		{
			co_await r.run(pool);

			auto result =
				co_await r.async_schedule<mock_module, bool>([](auto) -> asio::awaitable<bool> { co_return true; });

			BOOST_TEST(!result);
		},
		asio::use_future);

	std::thread t([&] { pool.run(); });

	future.get();

	pool.stop();

	t.join();
}
BOOST_AUTO_TEST_CASE(timer)
{
	module_router r{};

	asio::io_context io{};

	auto future = asio::co_spawn(
		io, [&]() -> asio::awaitable<void> { BOOST_REQUIRE_NO_THROW(co_await r.timer(10ms)); }, asio::use_future);

	io.run();

	future.get();
}

BOOST_AUTO_TEST_SUITE_END()
