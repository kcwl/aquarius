#define BOOST_TEST_NO_MAIN
#include <aquarius/ip/tcp/tcp_client.hpp>
#include <aquarius/ip/tcp/tcp_server.hpp>
#include <boost/test/unit_test.hpp>
#include "test.virgo.h"


template<typename Person>
void check_person(const Person& lhs, const Person& rhs)
{
	BOOST_TEST(lhs.addr == rhs.addr);
	BOOST_TEST(lhs.age == rhs.age);
	BOOST_TEST(lhs.hp == rhs.hp);
	BOOST_TEST(lhs.mana == rhs.mana);
	BOOST_TEST(lhs.name == rhs.name);
	BOOST_TEST(lhs.orders == lhs.orders);
	BOOST_TEST(lhs.score == rhs.score);
	BOOST_TEST(lhs.sex == rhs.sex);
	BOOST_TEST(lhs.telephone == rhs.telephone);
}

template <typename Server>
struct test_server_fixture
{
	test_server_fixture()
	{
		srv = std::make_shared<Server>(8100,3, "test server");

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

		BOOST_CHECK(client.get_executor() == io.get_executor());
		BOOST_TEST(client.get_timeout() == 1000ms);
	}

	{
		aquarius::tcp_client client(io.get_executor(), 1000ms);

		BOOST_CHECK(client.get_executor() == io.get_executor());
		BOOST_TEST(client.get_timeout() == 1000ms);
	}
}

BOOST_AUTO_TEST_CASE(tcp_client_flow)
{
	aquarius::io_context io;
	aquarius::tcp_client client(io, 300ms);

	int acc_success = 0;
	int clo_success = 0;

	client.set_accept_func([&](std::shared_ptr<aquarius::tcp_client_session>) { acc_success++; });
	client.set_close_func([&](std::shared_ptr<aquarius::tcp_client_session>) { clo_success++; });

	auto future = aquarius::co_spawn(
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

			auto req = std::make_shared<login_request>();

			auto resp = co_await client.async_send<login_response>(req);

			check_person(req->body().per_req, resp.body().per_resp);
		},
		aquarius::use_future);

	std::thread t([&] { io.run(); });

	auto status = future.wait_for(3000s);

	BOOST_CHECK(status == std::future_status::ready);

	client.close();

	io.stop();

	t.join();
}

BOOST_AUTO_TEST_SUITE_END()