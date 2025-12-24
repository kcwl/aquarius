#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/io_service_pool.hpp>
#include <aquarius/timer.hpp>

BOOST_AUTO_TEST_SUITE(io_pool)

using namespace std::chrono_literals;

BOOST_AUTO_TEST_CASE(ctor)
{
	aquarius::io_service_pool pool(1);

	BOOST_TEST(pool.pool_size() == 1);

	auto& io1 = pool.get_io_service();

	auto& io2 = pool.get_io_service();

	BOOST_CHECK(io1.get_executor() == io2.get_executor());

	aquarius::timer<aquarius::steady_timer> _timer(io1, 1s);
	_timer.async_wait([&](aquarius::error_code) { std::cout << "timers\n"; });

	std::thread t([&] { pool.run(); });
	
	std::this_thread::sleep_for(2s);

	BOOST_TEST(pool.enable());

	pool.stop();

	BOOST_TEST(!pool.enable());

	t.join();
}

BOOST_AUTO_TEST_CASE(io_service_pools)
{
	BOOST_CHECK_THROW(aquarius::io_service_pool pool(0), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()