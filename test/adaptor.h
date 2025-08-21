#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius/adaptor/timer_adaptor.hpp>
#include <aquarius/io_context.hpp>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(adaptors)

BOOST_AUTO_TEST_CASE(sql_adaptor_test)
{
	{

	}
}

BOOST_AUTO_TEST_CASE(timer_adaptor_test)
{
	aquarius::io_context io;

	aquarius::timer_adaptor<boost::asio::steady_timer> timer(io);

	int times = 0;

	timer.async_wait(100ms, [&] {times++;});

	std::thread t([&] {io.run(); });

	std::this_thread::sleep_for(1s);

	BOOST_CHECK_EQUAL(times,10);
}

BOOST_AUTO_TEST_SUITE_END()