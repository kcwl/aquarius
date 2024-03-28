#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(basic_server)

struct person_body_request
{
	bool sex;
	std::vector<uint8_t> role_data;
	double mana;
	float hp;
	int32_t age;
	int64_t money;
	std::string name;
	uint32_t back_money;
	uint64_t crc;

private:
	friend class elastic::access;

	template<typename _Archive>
	void serialize(_Archive& ar)
	{
		ar& sex;
		ar& role_data;
		ar& mana;
		ar& hp;
		ar& age;
		ar& money;
		ar& name;
		ar& back_money;
		ar& crc;
	}
};

struct person_body_response
{
	bool sex;
	std::vector<uint8_t> role_data;
	double mana;
	float hp;
	int32_t age;
	int64_t money;
	std::string name;
	uint32_t back_money;
	uint64_t crc;

private:
	friend class elastic::access;

	template<typename _Archive>
	void serialize(_Archive& ar)
	{
		ar& sex;
		ar& role_data;
		ar& mana;
		ar& hp;
		ar& age;
		ar& money;
		ar& name;
		ar& back_money;
		ar& crc;
	}
};

using person_request = aquarius::request<person_body_request, 10000>;
using person_response = aquarius::response<person_body_response, 10001>;

class ctx_test_server : public aquarius::context<person_request, person_response>
{
public:
	ctx_test_server()
		: aquarius::context<person_request, person_response>("ctx_test_server")
	{}

public:
	virtual aquarius::error_code handle() override
	{
		std::cout << "server ctx\n";
		response_.body().age = 1;
		response_.body().name = "hello";

		send_response(0);

		return aquarius::error_code{};
	}
};

AQUARIUS_CONTEXT_REGIST(person_request, ctx_test_server);

class ctx_test_client : public aquarius::content<person_response>
{
public:
	ctx_test_client()
		: aquarius::content<person_response>("ctx_test_client")
	{}

public:
	virtual aquarius::error_code handle() override
	{
		std::cout << "test response recved!\n";

		BOOST_CHECK_EQUAL(response_ptr_->body().age, 1);
		BOOST_CHECK_EQUAL(response_ptr_->body().name, "hello");

		return {};
	}
};

AQUARIUS_CONTEXT_REGIST(person_response, ctx_test_client);

BOOST_AUTO_TEST_CASE(process_message)
{
	aquarius::tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	person_request req{};
	req.body().age = 1;
	req.body().name = "world";

	cli.async_write(std::move(req), 
		[&](std::shared_ptr<person_response> resp)
		{
			BOOST_CHECK(true);
		});

	std::this_thread::sleep_for(1s);

	cli.stop();
	srv.stop();
	
	std::this_thread::sleep_for(1s);

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_CASE(handshake)
{
	aquarius::tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	cli.close();

	std::this_thread::sleep_for(1s);

	srv.stop();
	cli.stop();

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_CASE(read)
{
	aquarius::tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	person_request req{};
	req.body().age = 1;
	req.body().name = "world";
	cli.close();
	cli.async_write(std::move(req));

	std::this_thread::sleep_for(1s);

	cli.stop();
	srv.stop();

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_CASE(write)
{
	aquarius::tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	cli.close();

	cli.stop();
	srv.stop();

	std::this_thread::sleep_for(1s);

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_CASE(no_ssl)
{
	aquarius::no_ssl_tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::no_ssl_tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	person_request req{};
	req.body().age = 1;
	req.body().name = "world";
	cli.close(true);
	cli.async_write(std::move(req));

	std::this_thread::sleep_for(1s);
	cli.close();
	srv.stop();

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_SUITE_END()