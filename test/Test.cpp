#define BOOST_TEST_MODULE UnitTest
#ifdef _WIN32
#include <sdkddkver.h>
#define NOMINMAX
#endif
#include <boost/test/included/unit_test.hpp>
#include <aquarius.hpp>
#include "detail.h"
#include "handle_impl.hpp"
#include "protocol.h"
#include "serialize.h"
#include "sql.h"
#include "cmd_options.h"
#ifdef _DEBUG
#include <vld.h>
#endif


using namespace std::chrono_literals;

BOOST_AUTO_TEST_CASE(tcp_flow_with_ssl)
{
	aquarius::tcp_server srv(8100, 10, "async tcp server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::io_context io;

	auto cli = std::make_shared<aquarius::tcp_client>(io, 10s);
	
	aquarius::co_spawn(io, [cli] ->aquarius::awaitable<void>
					   {
						   auto is_connect = co_await cli->connect("127.0.0.1", "8100");

						   BOOST_CHECK(!is_connect);

						   auto req = std::make_shared<login_request>();
						   req->header().uuid = 1;
						   req->body().per_req.sex = true;
						   req->body().per_req.addr = 2;
						   req->body().per_req.age = 15;
						   req->body().per_req.telephone = 15230214856;
						   req->body().per_req.score = 100;
						   req->body().per_req.hp = 200;
						   req->body().per_req.mana = 300;
						   req->body().per_req.name = "John";
						   req->body().per_req.orders = { 1, 2, 3, 4, 5 };

						   auto resp = co_await cli->send<login_request, login_response>(req);

						   BOOST_CHECK_EQUAL(resp.header().uuid, req->header().uuid);
						   //BOOST_CHECK_EQUAL(resp.body().sex, req->body().sex);
						   //BOOST_CHECK_EQUAL(resp.body().addr, req->body().addr);
						   //BOOST_CHECK_EQUAL(resp.body().age, req->body().age);
						   //BOOST_CHECK_EQUAL(resp.body().telephone, req->body().telephone);
						   //BOOST_CHECK_EQUAL(resp.body().score, req->body().score);
						   //BOOST_CHECK_EQUAL(resp.body().hp, req->body().hp);
						   //BOOST_CHECK_EQUAL(resp.body().mana, req->body().mana);
						   //BOOST_CHECK_EQUAL(resp.body().info, req->body().info);
						   //BOOST_CHECK_EQUAL(resp.body().name, req->body().name);
						   //BOOST_CHECK_EQUAL(resp.body().orders, req->body().orders);
						   BOOST_CHECK_EQUAL(resp.body().per_resp, req->body().per_req); 

						   std::this_thread::sleep_for(5s);
					   }, aquarius::use_future);


	std::thread t1([&] { io.run(); });

	std::this_thread::sleep_for(10s);

	io.stop();

	srv.stop();

	t.join();
	t1.join();
}

BOOST_AUTO_TEST_CASE(http_flow_with_ssl)
{
	aquarius::http_server srv(8100, 10, "async http server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::io_context io;

	auto cli = std::make_shared<aquarius::http_client>(io, 10s);

	aquarius::co_spawn(io, [cli] ->aquarius::awaitable<void>
					   {
						   auto is_connect = co_await cli->connect("127.0.0.1", "8100");

						   BOOST_CHECK(!is_connect);

						   auto req = std::make_shared<http_login_request>();
						   req->header().uuid = 1;
						   req->body().per_req.sex = true;
						   req->body().per_req.addr = 2;
						   req->body().per_req.age = 15;
						   req->body().per_req.telephone = 15230214856;
						   req->body().per_req.score = 100;
						   req->body().per_req.hp = 200;
						   req->body().per_req.mana = 300;
						   req->body().per_req.name = "John";
						   req->body().per_req.orders = { 1, 2, 3, 4, 5 };

						   auto resp = co_await cli->send<http_login_request, http_login_response>(req);

						   BOOST_CHECK_EQUAL(resp.header().uuid, req->header().uuid);

						   BOOST_CHECK_EQUAL(resp.body().per_resp, req->body().per_req);

						   std::this_thread::sleep_for(5s);
					   }, aquarius::use_future);


	std::thread t1([&] { io.run(); });

	std::this_thread::sleep_for(10s);

	io.stop();

	srv.stop();

	t1.join();

	t.join();
}