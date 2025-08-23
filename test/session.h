#pragma once
#include <boost/test/unit_test_suite.hpp>
#include "protocol.hpp"
#include <aquarius.hpp>
#include <thread>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(session_test)

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