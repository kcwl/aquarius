#include <aquarius.hpp>
#include <boost/test/unit_test.hpp>
#include <thread>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(sessions)

//BOOST_AUTO_TEST_CASE(no_connect)
//{
//	aquarius::io_context io;
//	boost::asio::ip::tcp::socket _socket(io);
//
//	aquarius::tcp_client_session session(std::move(_socket), 30ms, 3s);
//
//	BOOST_TEST(session.uuid() != 0);
//
//	auto future = aquarius::co_spawn(
//		io,
//		[&]() -> aquarius::awaitable<void>
//		{
//			BOOST_TEST(!co_await session.async_connect("127.0.0.1", "9999"));
//
//			BOOST_TEST(session.remote_address_u() == 0u);
//
//			BOOST_TEST(session.close());
//		},
//		aquarius::use_future);
//
//	std::thread t([&] { io.run(); });
//
//	future.get();
//
//	io.stop();
//
//	t.join();
//}
//
//BOOST_AUTO_TEST_CASE(conn)
//{
//	aquarius::tcp_server srv(8100, 2);
//
//	std::thread t([&] { srv.run(); });
//
//	aquarius::io_context io;
//	boost::asio::ip::tcp::socket _socket(io);
//
//	aquarius::tcp_client_session session(std::move(_socket), 30ms, 3s);
//
//	auto future = aquarius::co_spawn(
//		io,
//		[&] () -> aquarius::awaitable<void>
//		{
//			BOOST_TEST(co_await session.async_connect("127.0.0.1", "8100"));
//
//			BOOST_TEST(session.set_nodelay());
//
//			BOOST_TEST(session.keep_alive());
//
//			BOOST_TEST(session.remote_address_u() == 2130706433u);
//
//			BOOST_TEST(session.close());
//		},
//		aquarius::use_future);
//
//	std::thread tc([&] { io.run(); });
//
//	future.get();
//
//	srv.stop();
//
//	io.stop();
//
//	t.join();
//	tc.join();
//}

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