#define BOOST_TEST_NO_MAIN
#include <aquarius/ip/tcp/tcp_server.hpp>
#include <boost/test/unit_test.hpp>
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

BOOST_AUTO_TEST_CASE(filter)
{
	aquarius::tcp_server server(8000, 1);

	aquarius::io_context io;

	BOOST_TEST(server.ip_filter(""));

	server.set_ip_filter([](const std::string& ip) { return false; });

	BOOST_TEST(!server.ip_filter(""));
}

BOOST_AUTO_TEST_CASE(callback)
{
	aquarius::tcp_server server(8000, 1);

	aquarius::io_context io;

	auto future = aquarius::co_spawn(
		io,
		[&]() -> aquarius::awaitable<void>
		{
			co_await server.accept_func(nullptr);

			server.set_accept_func([] (std::shared_ptr<aquarius::tcp_server_session>)->aquarius::awaitable<void> { BOOST_TEST(true); co_return; });

			co_await server.accept_func(nullptr);

			co_await server.close_func(nullptr);

			server.set_close_func([] (std::shared_ptr<aquarius::tcp_server_session>)->aquarius::awaitable<void> { BOOST_TEST(true); co_return; });

			co_await server.close_func(nullptr);
		},
		aquarius::use_future);

	io.run();

	future.get();

	io.stop();
}

BOOST_AUTO_TEST_SUITE_END()