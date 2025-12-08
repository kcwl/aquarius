#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/tcp/tcp_server.hpp>
#include <csignal>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(server)

BOOST_AUTO_TEST_CASE(signals)
{
	{
		aquarius::tcp_server srv(8888, 10, "signal[SIGINT] for server");

		std::thread t([&] { srv.run(); });

		std::this_thread::sleep_for(2s);

		std::raise(SIGINT);

		std::this_thread::sleep_for(2s);

		BOOST_TEST(!srv.enable());

		t.join();
	}

	{
		aquarius::tcp_server srv(9999, 10, "signal[SIGTERM] for server");

		std::thread t([&] { srv.run(); });

		std::this_thread::sleep_for(2s);

		std::raise(SIGTERM);

		std::this_thread::sleep_for(2s);

		BOOST_TEST(!srv.enable());

		t.join();
	}
}

BOOST_AUTO_TEST_SUITE_END()