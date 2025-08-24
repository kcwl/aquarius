#define BOOST_TEST_MODULE UnitTest
#ifdef _WIN32
#include <sdkddkver.h>
#ifdef USE_WIN7_FOR_BOOST_LOG
#define BOOST_USE_WINAPI_VERSION BOOST_WINAPI_VERSION_WIN7
#endif
#define NOMINMAX
#endif
#include <boost/test/included/unit_test.hpp>
#include "server.h"
#include "detail.h"
#include "protocol.hpp"
#include "session.h"
#include "adaptor.h"
#include "sql.h"
#include "attribute.h"


//BOOST_AUTO_TEST_CASE(tcp_flow_with_no_ssl)
//{
//	aquarius::tcp_server srv(8100, 10, "async tcp server");
//
//	std::thread t([&] { srv.run(); });
//
//	std::this_thread::sleep_for(2s);
//
//	auto cli = std::make_shared<aquarius::tcp_executor_client>(3, 10s);
//
//	auto future = aquarius::co_spawn(
//		cli->get_context(),
//		[&]() -> aquarius::awaitable<void>
//		{
//			auto is_connect = co_await cli->async_connect("127.0.0.1", "8100");
//
//			BOOST_CHECK(is_connect);
//
//			auto req = std::make_shared<rpc_test::request>();
//			req->header().uuid(1);
//			req->body().sex = true;
//			req->body().addr = 2;
//			req->body().age = 15;
//			req->body().telephone = 15230214856;
//			req->body().score = 100;
//			req->body().hp = 200;
//			req->body().mana = 300;
//			req->body().info = { 1, 1, 1, 1, 1, 1 };
//			req->body().name = "John";
//			req->body().orders = { 1, 2, 3, 4, 5 };
//
//			co_await cli->async_send<rpc_test>(req);
//
//			std::this_thread::sleep_for(5s);
//		},
//		aquarius::use_future);
//
//	std::thread tc([&] { cli->get_context().run(); });
//
//	future.get();
//
//	BOOST_CHECK_EQUAL(cli->remote_address(), "127.0.0.1");
//
//	BOOST_CHECK(cli->remote_address_u() != 0);
//
//	BOOST_CHECK_EQUAL(cli->remote_port(), 8100);
//
//	srv.stop();
//
//	t.join();
//
//	cli->stop();
//	tc.join();
//}

BOOST_AUTO_TEST_CASE(tcp_flow_with_ssl)
{
	aquarius::ssl_tcp_server srv(8100, 10, "async tcp server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	auto cli = std::make_shared<aquarius::ssl_tcp_executor_client>(3, 10s);
	
	aquarius::co_spawn(cli->get_context(), [cli] ->aquarius::awaitable<void>
					   {
						   auto is_connect = co_await cli->async_connect("127.0.0.1", "8100");

						   BOOST_CHECK(is_connect);

						   auto req = std::make_shared<rpc_test::request>();
						   req->header().uuid(1);
						   req->body().sex = true;
						   req->body().addr = 2;
						   req->body().age = 15;
						   req->body().telephone = 15230214856;
						   req->body().score = 100;
						   req->body().hp = 200;
						   req->body().mana = 300;
						   req->body().info = { 1, 1, 1, 1, 1, 1 };
						   req->body().name = "John";
						   req->body().orders = { 1, 2, 3, 4, 5 };

						   co_await cli->async_send<rpc_test>(req);

						   std::this_thread::sleep_for(5s);
					   }, aquarius::use_future);


	std::thread t1([&] { cli->get_context().run(); });

	std::this_thread::sleep_for(5s);

	cli->stop();

	srv.stop();

	t.join();
	t1.join();
}