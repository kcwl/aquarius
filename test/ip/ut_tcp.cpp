#include "login.virgo.h"
#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ut_tcp)

BOOST_AUTO_TEST_CASE(flow)
{
	aquarius::tcp::server srv(8100, 10, "async tcp server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::asio::io_context io;

	auto cli = std::make_shared<aquarius::tcp::client>(io, 10s);

	auto future = aquarius::asio::co_spawn(
		io,
		[cli] -> aquarius::asio::awaitable<void>
		{
			auto is_connect = co_await cli->async_connect("127.0.0.1", 8100);

			BOOST_CHECK(!is_connect);

			auto req = std::make_shared<login_request>();
			req->header().uuid(1);
			req->body().username() = "login";
			req->body().password() = "password";

			auto resp = co_await cli->async_call<login_response>(req);

			BOOST_TEST(resp.result() == 0);
		},
		aquarius::asio::use_future);

	std::thread t1([&] { io.run(); });

	auto status = future.wait_for(10s);

	BOOST_REQUIRE(status == std::future_status::ready);

	cli->close();

	srv.stop();

	t.join();
	t1.join();
}

BOOST_AUTO_TEST_SUITE_END()