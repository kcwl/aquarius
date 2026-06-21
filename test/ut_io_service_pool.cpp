#include <aquarius/io_service_pool.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ut_io_service_pool)

using namespace aquarius;
using namespace std::chrono_literals;

BOOST_AUTO_TEST_CASE(ctor)
{
	io_service_pool io(10);

	BOOST_TEST(io.capacity() == 10);

	BOOST_REQUIRE_THROW(io_service_pool zero(0), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(run)
{
	io_service_pool io(10);

	std::thread t([&] { io.run(); });

	std::this_thread::sleep_for(2s);

	BOOST_TEST(io.enable());

	io.stop();

	BOOST_TEST(!io.enable());
	t.join();
}
BOOST_AUTO_TEST_SUITE_END()
