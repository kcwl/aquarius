#pragma once
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(sconnect)

BOOST_AUTO_TEST_CASE(ssl)
{
	//{
	//	aquarius::async_tcp_server srv(8100, 2);

	//	std::thread t([&] { srv.run(); });

	//	std::promise<bool> connect_result{};

	//	auto future = connect_result.get_future();

	//	aquarius::async_tcp_client cli("127.0.0.1", "8100", [&](auto result) { connect_result.set_value(result); });

	//	std::thread tc([&] { cli.run(); });

	//	auto status = future.wait_for(5s);

	//	BOOST_CHECK(status == std::future_status::ready);

	//	BOOST_CHECK(future.get());

	//	std::this_thread::sleep_for(1s);

	//	srv.stop();
	//	cli.stop();

	//	t.join();
	//	tc.join();
	//}

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

//BOOST_AUTO_TEST_CASE(large_pack)
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
//}

class test_request
{
public:
	constexpr static auto Number = 1001;
public:
	test_request()
		: content_()
	{

	}

public:
	template<typename Buffer>
	Buffer to_binary()
	{
		Buffer fs;

		fs.save((uint8_t*)content_.data(), content_.size());

		return fs;
	}

	bool from_binary(aquarius::flex_buffer_t& fs)
	{
		content_.resize(fs.size());

		fs.load((uint8_t*)content_.data(), content_.size());

		return true;
	}

public:
	std::string content_;
};

class test_response
{
public:
	constexpr static auto Number = 1002;
public:
	test_response()
		: content_()
	{

	}

public:
	template<typename Buffer>
	Buffer to_binary()
	{
		Buffer fs;

		fs.save((uint8_t*)content_.data(), content_.size());

		return fs;
	}


	bool from_binary(aquarius::flex_buffer_t& fs)
	{
		content_.resize(fs.size());

		fs.load((uint8_t*)content_.data(), content_.size());

		return true;
	}

public:
	std::string content_;
};

AQUARIUS_SERVER_CONTEXT(ctx_test, test_request, test_response)
{
	response().content_ = message()->content_;

	return 0;
}

AQUARIUS_CLIENT_CONTEXT(ctn_test, test_response)
{
	std::cout << "client recv!\n";

	return 0;
}

BOOST_AUTO_TEST_CASE(connect_with_no_ssl)
{
	aquarius::async_tcp_server srv(8100, 10, "async tcp server");

	std::thread t([&]
		{
			srv.run();
		});

	std::this_thread::sleep_for(2s);

	aquarius::async_tcp_client cli("127.0.0.1", "8100");

	std::thread t1([&] {cli.run(); });

	test_request req{};
	req.content_ = "hello world!";

	cli.send_request(req);

	std::this_thread::sleep_for(500s);

	cli.stop();

	srv.stop();

	t.join();
	t1.join();
}

BOOST_AUTO_TEST_SUITE_END()