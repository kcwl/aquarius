#pragma once
#include <boost/test/unit_test_suite.hpp>


struct person
{
	bool sex;
	uint32_t addr;
	int32_t age;
	uint64_t telephone;
	int64_t score;
	float hp;
	double mana;
	std::vector<uint8_t> info;
	std::string name;
	std::vector<int> orders;

	void swap(person& other)
	{
		std::swap(sex, other.sex);
		std::swap(addr, other.addr);
		std::swap(age, other.age);
		std::swap(telephone, other.telephone);
		std::swap(score, other.score);
		std::swap(hp, other.hp);
		std::swap(mana, other.mana);
		std::swap(info, other.info);
		std::swap(name, other.name);
		std::swap(orders, other.orders);
	}
};

template <>
struct aquarius::reflect<person>
{
	using value_type = person;

	constexpr static std::string_view topic()
	{
		return "person"sv;
	}

	constexpr static std::array<std::string_view, 10> fields()
	{
		return {
			"sex"sv, "addr"sv, "age"sv, "telephone"sv, "score"sv, "hp"sv, "mana"sv, "info"sv, "name"sv, "orders"sv
		};
	}
};

bool operator==(const person& lhs, const person& rhs)
{
	return lhs.sex == rhs.sex && lhs.addr == rhs.addr && lhs.age == rhs.age && lhs.telephone == rhs.telephone &&
		lhs.score == rhs.score && lhs.hp == rhs.hp && lhs.mana && rhs.mana && lhs.info == rhs.info &&
		lhs.name == rhs.name && lhs.orders == rhs.orders;
}

std::ostream& operator<<(std::ostream& os, const person& p)
{
	os << p.sex << "," << p.addr << "," << p.age << "," << p.telephone << "," << p.score << "," << p.hp << "," << p.mana
		<< ", [";

	for (auto& v : p.info)
	{
		os << v << ",";
	}

	os.seekp(-1, std::ios::cur);

	os << "]," << p.name << ", [";

	for (auto& v : p.info)
	{
		os << v << ",";
	}

	os.seekp(-1, std::ios::cur);

	os << "]\n";

	return os;
}

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


using test_request = aquarius::ip::tcp::request<person, 1001>;

using test_response = aquarius::ip::tcp::response<person, 1002>;

AQUARIUS_SERVER_CONTEXT(ctx_test, test_request, test_response)
{
	response().body() = message()->body();

	return 0;
}

AQUARIUS_CLIENT_CONTEXT(ctn_test, test_response)
{
	std::cout << "client recv!\n";

	return 0;
}

BOOST_AUTO_TEST_CASE(connect_with_no_ssl)
{
	aquarius::tcp::async_server srv(8100, 10, "async tcp server");

	std::thread t([&]
		{
			srv.run();
		});

	std::this_thread::sleep_for(2s);

	aquarius::tcp::async_client cli("127.0.0.1", "8100");

	std::thread t1([&] {cli.run(); });

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
BOOST_AUTO_TEST_CASE(connect_with_ssl)
{
	aquarius::ssl::tcp_server srv(8100, 10, "async tcp server");

	std::thread t([&]
		{
			srv.run();
		});

	std::this_thread::sleep_for(2s);

	aquarius::ssl::tcp_client cli("127.0.0.1", "8100");

	std::thread t1([&] {cli.run(); });

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

BOOST_AUTO_TEST_SUITE_END()