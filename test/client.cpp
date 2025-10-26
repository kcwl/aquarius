#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/tcp/tcp_client.hpp>
#include <aquarius/ip/tcp/tcp_server.hpp>

BOOST_AUTO_TEST_SUITE(client_test)

BOOST_AUTO_TEST_CASE(client_tcp)
{
	aquarius::tcp_server srv(8100, 10, "async tcp server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::io_context io;

	auto cli = std::make_shared<aquarius::tcp_client>(io, "127.0.0.1", "8100");

	auto future = aquarius::co_spawn(
		io,
		[cli] -> aquarius::awaitable<void>
		{
			auto is_connect = co_await cli->async_connect();

			BOOST_CHECK(!is_connect);

			cli->close();

			is_connect = co_await cli->reconnect();

			BOOST_CHECK(!is_connect);
		},
		aquarius::use_future);

	std::thread t1([&] { io.run(); });

	auto status = future.wait_for(5s);

	BOOST_CHECK(status == std::future_status::ready);

	io.stop();

	srv.stop();

	t.join();
	t1.join();
}

BOOST_AUTO_TEST_SUITE_END()