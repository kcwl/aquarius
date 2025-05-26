#define BOOST_TEST_MODULE UnitTest
#include <boost/test/included/unit_test.hpp>
#include "protocol.hpp"
#include "detail.h"

BOOST_AUTO_TEST_CASE(tcp_flow_with_no_ssl)
{
	aquarius::tcp::async_server srv(8100, 10, "async tcp server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::tcp::async_client cli("127.0.0.1", "8100");

	std::thread t1([&] { cli.run(); });

	test_request req{};
	req.header()->crc32_ = 1;
	req.header()->timestamp_ = 1;
	req.body().sex = true;
	req.body().addr = 2;
	req.body().age = 15;
	req.body().telephone = 15230214856;
	req.body().score = 100;
	req.body().hp = 200;
	req.body().mana = 300;
	req.body().info = { 1, 1, 1, 1, 1, 1 };
	req.body().name = "John";
	req.body().orders = { 1, 2, 3, 4, 5 };

	cli.send_request(req);

	std::this_thread::sleep_for(5s);

	cli.stop();

	srv.stop();

	t.join();
	t1.join();
}

#ifdef AQUARIUS_ENABLE_SSL
BOOST_AUTO_TEST_CASE(tcp_flow_with_ssl)
{
	aquarius::ssl::tcp_server srv(8100, 10, "async tcp server");

	std::thread t([&] { srv.run(); });

	std::this_thread::sleep_for(2s);

	aquarius::ssl::tcp_client cli("127.0.0.1", "8100");

	std::thread t1([&] { cli.run(); });

	test_request req{};
	req.header()->crc32_ = 1;
	req.header()->timestamp_ = 1;
	req.body().sex = true;
	req.body().addr = 2;
	req.body().age = 15;
	req.body().telephone = 15230214856;
	req.body().score = 100;
	req.body().hp = 200;
	req.body().mana = 300;
	req.body().info = { 1, 1, 1, 1, 1, 1 };
	req.body().name = "John";
	req.body().orders = { 1, 2, 3, 4, 5 };

	cli.send_request(req);

	std::this_thread::sleep_for(5s);

	cli.stop();

	srv.stop();

	t.join();
	t1.join();
}
#endif