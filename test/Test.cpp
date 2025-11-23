#define BOOST_TEST_MODULE UnitTest
#include <boost/test/unit_test.hpp>
//#include <aquarius.hpp>
#include "ctx_handler.hpp"

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(framework)


BOOST_AUTO_TEST_CASE(tcp_flow)
{
	aquarius::tcp_server srv(8100, 10, "async tcp server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::io_context io;

	auto cli = std::make_shared<aquarius::tcp_client>(io, 10s);

	auto future = aquarius::co_spawn(
		io,
		[cli] -> aquarius::awaitable<void>
		{
			auto is_connect = co_await cli->async_connect("127.0.0.1", "8100");

			BOOST_CHECK(!is_connect);

			auto req = std::make_shared<login_tcp_request>();
			req->header().uuid_ = 1;
			req->body().per_req.sex = true;
			req->body().per_req.addr = 2;
			req->body().per_req.age = 15;
			req->body().per_req.telephone = 15230214856;
			req->body().per_req.score = 100;
			req->body().per_req.hp = 200;
			req->body().per_req.mana = 300;
			req->body().per_req.name = "John";
			req->body().per_req.orders = { 1, 2, 3, 4, 5 };

			auto resp = co_await cli->query<login_tcp_response>(req);

			BOOST_TEST(resp.header().uuid_ == req->header().uuid_);
			BOOST_TEST(resp.body().per_resp == req->body().per_req);

			BOOST_TEST(cli->remote_address() == "127.0.0.1");
			BOOST_TEST(cli->remote_port() == 8100);
		},
		aquarius::use_future);

	std::thread t1([&] { io.run(); });

	auto status = future.wait_for(20s);

	BOOST_CHECK(status == std::future_status::ready);

	io.stop();

	srv.stop();

	t.join();
	t1.join();
}

BOOST_AUTO_TEST_CASE(http_post_flow)
{
	aquarius::http_server srv(80, 10, "async http post server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::io_context io;

	auto cli = std::make_shared<aquarius::http_client>(io, 10s);

	auto future = aquarius::co_spawn(
		io,
		[cli] -> aquarius::awaitable<void>
		{
			auto is_connect = co_await cli->async_connect("127.0.0.1", "80");

			BOOST_TEST(!is_connect);

			if (is_connect)
				co_return;

			auto req = std::make_shared<new_http_login_http_request>();
			req->body().uuid = 1;
			req->body().per_req.sex = true;
			req->body().per_req.addr = 2;
			req->body().per_req.age = 15;
			req->body().per_req.telephone = 15230214856;
			req->body().per_req.score = 100;
			req->body().per_req.hp = 200;
			req->body().per_req.mana = 300;
			req->body().per_req.name = "John";
			req->body().per_req.orders = { 1, 2, 3, 4, 5 };

			auto resp = co_await cli->post<new_http_login_http_response>(req);

			BOOST_TEST(resp.body().uuid == req->body().uuid);

			BOOST_TEST(resp.body().per_resp == req->body().per_req);

			BOOST_TEST(cli->remote_address() == "127.0.0.1");
			BOOST_TEST(cli->remote_port() == 80);
		},
		aquarius::use_future);

	std::thread t1([&] { io.run(); });

	auto status = future.wait_for(10s);

	BOOST_CHECK(status == std::future_status::ready);

	io.stop();

	srv.stop();

	t1.join();

	t.join();
}

BOOST_AUTO_TEST_CASE(http_get_flow)
{
	aquarius::http_server srv(8080, 10, "async http get server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::io_context io;

	auto cli = std::make_shared<aquarius::http_client>(io, 10s);

	auto future = aquarius::co_spawn(
		io,
		[cli] -> aquarius::awaitable<void>
		{
			auto is_connect = co_await cli->async_connect("127.0.0.1", "8080");

			BOOST_TEST(!is_connect);

			if (is_connect)
				co_return;

			auto req = std::make_shared<http_test_get_http_request>();
			req->body().user = 12345;
			req->body().passwd = "passwd123";

			auto resp = co_await cli->get<http_test_get_http_response>(req);

			BOOST_TEST(resp.body().user == req->body().user);

			BOOST_TEST(resp.body().passwd == req->body().passwd);
		},
		aquarius::use_future);

	std::thread t1([&] { io.run(); });

	auto status = future.wait_for(10s);

	BOOST_CHECK(status == std::future_status::ready);

	io.stop();

	srv.stop();

	t1.join();

	t.join();
}

BOOST_AUTO_TEST_SUITE_END()