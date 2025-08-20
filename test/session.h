#pragma once
#include "protocol.hpp"
#include <aquarius.hpp>
#include <boost/test/unit_test_suite.hpp>


BOOST_AUTO_TEST_SUITE(session_test)

BOOST_AUTO_TEST_CASE(remote_info)
{
	{
		aquarius::io_context io;

		aquarius::tcp_server srv(8100, 2);

		std::thread t([&] { srv.run(); });

		aquarius::tcp_client cli(io, 3, 10s);

		auto future = aquarius::co_spawn(
			io,
			[&cli] -> aquarius::awaitable<bool>
			{
				auto result = co_await cli.async_connect("127.0.0.1", "8100");

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

				co_await cli.async_send<rpc_test>(req);

				co_return result;
			},
			aquarius::use_future);

		std::thread tc([&] { io.run(); });

		BOOST_CHECK(future.get());

		BOOST_CHECK_EQUAL(cli.remote_address(), "127.0.0.1");

		BOOST_CHECK_EQUAL(cli.remote_port(), 8100);

		std::this_thread::sleep_for(5s);
		cli.stop();
		io.stop();
		srv.stop();
		
		

		t.join();
		tc.join();
	}

	//{
	//	aquarius::async_tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	std::promise<bool> connect_result{};
	//	auto future = connect_result.get_future();

	//	aquarius::tcp_client cli("127.0.0.1", "8100", [&](auto result) { connect_result.set_value(result); });

	//	std::thread tc([&] { cli.run(); });

	//	srv.stop();
	//	t.join();

	//	BOOST_CHECK(future.get());

	//	cli.stop();

	//	tc.join();
	//}

	//{
	//	aquarius::tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	aquarius::tcp_client cli("127.0.0.1", "8100");

	//	std::thread tc([&] { cli.run(); });

	//	std::this_thread::sleep_for(1s);

	//	aquarius::flex_buffer_t fs{};
	//	uint8_t a = '1';
	//	fs.save(&a, 1);
	//	cli.async_write(std::move(fs));

	//	std::this_thread::sleep_for(1s);

	//	cli.stop();
	//	srv.stop();

	//	t.join();
	//	tc.join();
	//}

	//{
	//	aquarius::tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	aquarius::tcp_client cli("127.0.0.1", "8100");

	//	std::thread tc([&] { cli.run(); });

	//	std::this_thread::sleep_for(1s);

	//	cli.close();

	//	cli.stop();
	//	srv.stop();

	//	std::this_thread::sleep_for(1s);

	//	t.join();
	//	tc.join();
	//}
}

BOOST_AUTO_TEST_CASE(no_ssl)
{
	//{
	//	aquarius::async_tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	aquarius::async_tcp_client cli("127.0.0.1", "8100");

	//	std::thread tc([&] { cli.run(); });

	//	std::this_thread::sleep_for(1s);

	//	srv.stop();
	//	cli.stop();

	//	t.join();
	//	tc.join();
	//}

	//{
	//	aquarius::async_tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	aquarius::async_tcp_client cli("127.0.0.1", "8100");

	//	std::thread tc([&] { cli.run(); });

	//	std::this_thread::sleep_for(1s);

	//	aquarius::flex_buffer_t fs{};
	//	uint8_t a = '1';
	//	//fs.save(&a, 1);
	//	//cli.close();
	//	cli.async_write(std::move(fs));

	//	std::this_thread::sleep_for(1s);
	//	cli.stop();
	//	srv.stop();

	//	t.join();
	//	tc.join();
	//}

	//{
	//	aquarius::async_tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	aquarius::async_tcp_client cli("127.0.0.1", "8100");

	//	std::thread tc([&] { cli.run(); });

	//	//cli.close();
	//	//cli.close();

	//	std::this_thread::sleep_for(1s);

	//	srv.stop();
	//	cli.stop();

	//	t.join();
	//	tc.join();
	//}

	//{
	//	aquarius::no_ssl_tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	aquarius::no_ssl_tcp_client cli("127.0.0.1", "8100");

	//	std::thread tc([&] { cli.run(); });

	//	std::this_thread::sleep_for(1s);

	//	aquarius::flex_buffer_t fs{};
	//	uint8_t a = '1';
	//	fs.save(&a, 1);
	//	cli.async_write(std::move(fs));

	//	std::this_thread::sleep_for(1s);

	//	cli.stop();
	//	srv.stop();

	//	t.join();
	//	tc.join();
	//}

	//{
	//	aquarius::async_tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	aquarius::async_tcp_client cli("127.0.0.1", "8100");

	//	std::thread tc([&] { cli.run(); });

	//	std::this_thread::sleep_for(1s);

	//	//cli.close();

	//	cli.stop();
	//	srv.stop();

	//	std::this_thread::sleep_for(1s);

	//	t.join();
	//	tc.join();
	//}
}

// BOOST_AUTO_TEST_CASE(sconnect)
//{
//	aquarius::no_ssl_tcp_server srv(8100, 2);
//
//	std::thread t([&] { srv.run(); });
//
//	aquarius::no_ssl_tcp_client cli("127.0.0.1", "8100");
//
//	std::thread tc([&] { cli.run(); });
//
//	std::this_thread::sleep_for(1s);
//
//	cli.stop();
//	aquarius::flex_buffer_t fs{};
//	uint8_t a = '1';
//	fs.save(&a, 1);
//	cli.async_write(std::move(fs));
//
//	std::this_thread::sleep_for(1s);
//
//	cli.stop();
//	srv.stop();
//
//	t.join();
//	tc.join();
// }

// BOOST_AUTO_TEST_CASE(large_pack)
//{
//	aquarius::async_tcp_server srv(8100, 2);
//
//	std::thread t([&] { srv.run(); });
//
//	aquarius::async_tcp_client cli("127.0.0.1", "8100");
//
//	std::thread tc([&] { cli.run(); });
//
//	std::this_thread::sleep_for(1s);
//
//	//person_request req{};
//	//req.body().age = 1;
//
//	//for (int i = 0; i < 3 * 4096; ++i)
//	//{
//	//	req.body().name.append("a");
//	//}
//
//	//cli.send_request(std::move(req), [&](std::shared_ptr<person_response> resp) { BOOST_CHECK(true); });
//
//	std::this_thread::sleep_for(1s);
//
//	cli.stop();
//	srv.stop();
//
//	std::this_thread::sleep_for(1s);
//
//	t.join();
//	tc.join();
// }

BOOST_AUTO_TEST_SUITE_END()