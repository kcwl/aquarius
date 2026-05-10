#define BOOST_TEST_MODULE UnitTest
#include "test_http.virgo.h"
#include "test_tcp.virgo.h"
#include <aquarius.hpp>
#include <aquarius/http.hpp>
#include <aquarius/tcp.hpp>
#include <boost/test/unit_test.hpp>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(framework)

struct person
{
	int id;
	int age;
};

BOOST_AUTO_TEST_CASE(tcp_flow)
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

			auto req = std::make_shared<login_tcp_request>();
			req->header().uuid(1);
			req->body().per_req().sex = true;
			req->body().per_req().addr = 2;
			req->body().per_req().age = 15;
			req->body().per_req().telephone = 15230214856;
			req->body().per_req().score = 100;
			req->body().per_req().hp = 200;
			req->body().per_req().mana = 300;
			req->body().per_req().name = "John";
			req->body().per_req().orders = { 1, 2, 3, 4, 5 };

			auto resp = co_await cli->async_call<login_tcp_response>(req);

			BOOST_TEST(resp.header().uuid() == req->header().uuid());
			BOOST_TEST(resp.body().per_resp().sex == req->body().per_req().sex);
			BOOST_TEST(resp.body().per_resp().addr == req->body().per_req().addr);
			BOOST_TEST(resp.body().per_resp().age == req->body().per_req().age);
			BOOST_TEST(resp.body().per_resp().sage == req->body().per_req().sage);
			BOOST_TEST(resp.body().per_resp().telephone == req->body().per_req().telephone);
			BOOST_TEST(resp.body().per_resp().score == req->body().per_req().score);
			BOOST_TEST(resp.body().per_resp().scores == req->body().per_req().scores);
			BOOST_TEST(resp.body().per_resp().f.value == req->body().per_req().f.value);
			BOOST_TEST(resp.body().per_resp().g.value == req->body().per_req().g.value);
			BOOST_TEST(resp.body().per_resp().aa == req->body().per_req().aa);
			BOOST_TEST(resp.body().per_resp().hp == req->body().per_req().hp);
			BOOST_TEST(resp.body().per_resp().mana == req->body().per_req().mana);
			BOOST_TEST(resp.body().per_resp().name == req->body().per_req().name);
			BOOST_TEST(resp.body().per_resp().orders == req->body().per_req().orders);
			BOOST_TEST(resp.body().per_resp().ves == req->body().per_req().ves);
		},
		aquarius::asio::use_future);

	std::thread t1([&] { io.run(); });

	auto status = future.wait_for(10s);

	BOOST_REQUIRE(status == std::future_status::ready);

	io.stop();

	srv.stop();

	t.join();
	t1.join();
}

BOOST_AUTO_TEST_CASE(http_post_flow)
{
	aquarius::http::server srv(8099, 10, "async http post server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::asio::io_context io;

	auto cli = std::make_shared<aquarius::http::client>(io, 10s);

	auto future = aquarius::asio::co_spawn(
		io,
		[cli] -> aquarius::asio::awaitable<void>
		{
			auto is_connect = co_await cli->async_connect("127.0.0.1", 8099);

			BOOST_TEST(!is_connect);

			auto req = std::make_shared<http_test_post_http_request>();
			req->method(http_method::post);
			req->body().uuid() = 1;
			req->body().per_req().sex = true;
			req->body().per_req().addr = 2;
			req->body().per_req().age = 15;
			req->body().per_req().telephone = 15230214856;
			req->body().per_req().score = 100;
			req->body().per_req().hp = 200;
			req->body().per_req().mana = 300;
			req->body().per_req().name = "John";
			req->body().per_req().orders = { 1, 2, 3, 4, 5 };

			auto resp = co_await cli->async_call<http_test_post_http_response>(req);

			BOOST_TEST(resp.body().uuid() == req->body().uuid());

			BOOST_TEST(resp.body().per_resp().sex == req->body().per_req().sex);
			BOOST_TEST(resp.body().per_resp().addr == req->body().per_req().addr);
			BOOST_TEST(resp.body().per_resp().age == req->body().per_req().age);
			BOOST_TEST(resp.body().per_resp().telephone == req->body().per_req().telephone);
			BOOST_TEST(resp.body().per_resp().score == req->body().per_req().score);
			BOOST_TEST(resp.body().per_resp().hp == req->body().per_req().hp);
			BOOST_TEST(resp.body().per_resp().mana == req->body().per_req().mana);
			BOOST_TEST(resp.body().per_resp().name == req->body().per_req().name);
			BOOST_TEST(resp.body().per_resp().orders == req->body().per_req().orders);
		},
		aquarius::asio::use_future);

	std::thread t1([&] { io.run(); });

	auto status = future.wait_for(10s);

	BOOST_REQUIRE(status == std::future_status::ready);

	io.stop();

	srv.stop();

	t1.join();

	t.join();
}

BOOST_AUTO_TEST_CASE(http_get_flow)
{
	aquarius::http::server srv(8080, 10, "async http get server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::asio::io_context io;

	auto cli = std::make_shared<aquarius::http::client>(io, 10s);

	auto future = aquarius::asio::co_spawn(
		io,
		[cli] -> aquarius::asio::awaitable<void>
		{
			auto is_connect = co_await cli->async_connect("127.0.0.1", 8080);

			BOOST_TEST(!is_connect);

			auto req = std::make_shared<http_test_get_http_request>();
			req->method(http_method::get);
			req->body().user() = 12345;
			req->body().passwd() = "passwd123";

			auto resp = co_await cli->async_call<http_test_get_http_response>(req);

			BOOST_TEST(resp.body().user() == req->body().user());

			BOOST_TEST(resp.body().passwd() == req->body().passwd());
		},
		aquarius::asio::use_future);

	std::thread t1([&] { io.run(); });

	auto status = future.wait_for(10s);

	BOOST_REQUIRE(status == std::future_status::ready);

	io.stop();

	srv.stop();

	t1.join();

	t.join();
}

#ifdef ENABLE_MYSQL

BOOST_AUTO_TEST_CASE(connecting)
{
	person p{ 1, 1 };

	aquarius::io_service_pool pool(1);

	aquarius::module_router<>::get_mutable_instance().regist<aquarius::mysql_module>();

	aquarius::module_router<>::get_mutable_instance().run(pool);

	std::this_thread::sleep_for(2s);

	auto future = aquarius::asio::co_spawn(
		pool.get_io_service(),
		[&]() -> aquarius::asio::awaitable<void>
		{
			auto res = co_await (aquarius::insert_v(p) | aquarius::enter);

			BOOST_TEST(res != 0);

			auto select_res = co_await (aquarius::select_v<person>() | aquarius::enter);

			BOOST_TEST(select_res.size() != 0);
		},
		aquarius::asio::use_future);

	std::thread t([&] { pool.run(); });

	std::this_thread::sleep_for(2s);

	future.get();

	pool.stop();

	t.join();
}

#endif

BOOST_AUTO_TEST_SUITE_END()