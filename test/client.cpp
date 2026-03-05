#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/ip/tcp/tcp_client.hpp>
#include <aquarius/ip/tcp/tcp_server.hpp>


template <typename Server>
struct test_server_fixture
{
	test_server_fixture()
	{
		srv = std::make_shared<Server>(3, 8100, "test server");

		t = std::make_shared<std::thread>([&] { srv->run(); });
	}

	~test_server_fixture()
	{
		srv->stop();
		t->join();
	}

	std::shared_ptr<Server> srv;
	std::shared_ptr<std::thread> t;
};

BOOST_AUTO_TEST_SUITE(tcp_client_test_suite, *boost::unit_test::fixture<test_server_fixture<aquarius::tcp_server>>())

BOOST_AUTO_TEST_CASE(ctor)
{
	aquarius::io_context io;
	{
		aquarius::tcp_client client(io, 1000ms);

		BOOST_TEST(client.get_executor() == io.get_executor());
		BOOST_TEST(client.get_timeout() == 1000ms);
	}

	{
		aquarius::tcp_client client(io.get_executor(), 1000ms);

		BOOST_TEST(client.get_executor() == io.get_executor());
		BOOST_TEST(client.get_timeout() == 1000ms);
	}
}

BOOST_AUTO_TEST_CASE(tcp_client_flow)
{
	aquarius::io_context io;
	aquarius::tcp_client client(io, 300ms);

	int acc_success = 0;
	int clo_success = 0;

	client.set_accept_func([&] { acc_success++; });
	client.set_close_func([&] { clo_success++; });

	aquarius::co_spawn(
		io,
		[&] -> aquarius::awaitable<void>
		{
			auto ec = co_await client.async_connect("", 0);
			BOOST_TEST(ec);

			ec = co_await client.async_connect("127.0.0.1", 8100);
			BOOST_TEST(!ec);

			ec = co_await client.reconnect();
			BOOST_TEST(!ec);

			BOOST_CHECK_EQUAL(client.remote_address(), "127.0.0.1");
			BOOST_CHECK_EQUAL(client.remote_port(), 8100);

			auto req = std::make_shared<test_request>();

			auto resp = co_await client.async_send<test_response>(req);

			BOOST_TEST(req, resp);
		},
		aquarius::detached);

	io.run();
}

BOOST_AUTO_TEST_SUITE_END()