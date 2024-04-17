#pragma once
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "proto_regist.h"

BOOST_AUTO_TEST_SUITE(message)

BOOST_AUTO_TEST_CASE(construction)
{
	{
		person_request req{};
		req.body().age = 3;
		req.body().back_money = 4;
		req.add_length(2);

		person_request req1(std::move(req));

		BOOST_CHECK(req.header() == nullptr);

		BOOST_CHECK(req1.body().age == 3 && req.body().age == 0);

		BOOST_CHECK(req1.body().back_money == 4 && req.body().back_money == 0);

		BOOST_CHECK(req.length() == 0);

		BOOST_CHECK(req.uuid() == 0);
	}

	{
		person_request req{};
		req.body().age = 3;
		req.body().back_money = 4;
		req.add_length(2);

		person_request req1{};
		req1 = std::move(req);

		BOOST_CHECK(req.header() == nullptr);

		BOOST_CHECK(req1.body().age == 3 && req.body().age == 0);

		BOOST_CHECK(req1.body().back_money == 4 && req.body().back_money == 0);

		BOOST_CHECK(req.length() == 0);

		BOOST_CHECK(req.uuid() == 0);
	}

	{
		person_response resp{};

		resp.body().age = 3;
		resp.body().back_money = 4;

		person_response resp1(std::move(resp));

		BOOST_CHECK(resp.header() == nullptr);

		BOOST_CHECK(resp1.body().age == 3 && resp.body().age == 0);

		BOOST_CHECK(resp1.body().back_money == 4 && resp.body().back_money == 0);

		BOOST_CHECK(resp.length() == 0);

		BOOST_CHECK(resp.uuid() == 0);
	}

	{
		person_response resp{};

		resp.body().age = 3;
		resp.body().back_money = 4;

		person_response resp1{};
		resp1 = std::move(resp);

		BOOST_CHECK(resp.header() == nullptr);

		BOOST_CHECK(resp1.body().age == 3 && resp.body().age == 0);

		BOOST_CHECK(resp1.body().back_money == 4 && resp.body().back_money == 0);

		BOOST_CHECK(resp.length() == 0);

		BOOST_CHECK(resp.uuid() == 0);
	}
}

BOOST_AUTO_TEST_CASE(from_and_to_binary)
{
	{
		aquarius::flex_buffer_t buffer{};
		person_request req{};
		req.header()->session_id = 1;
		req.body().age = 3;

		BOOST_CHECK(req.to_binary(buffer));

		std::size_t proto{};
		elastic::from_binary(proto, buffer);

		person_request req1{};

		BOOST_CHECK(req1.from_binary(buffer));
	}

	{
		aquarius::flex_buffer_t buffer{};

		elastic::to_binary(13, buffer);

		person_request req{};

		req.from_binary(buffer);

		BOOST_CHECK(buffer.size() == 1);
	}

	//{
	//	aquarius::flex_buffer_t buffer(0);

	//	person_request req{};
	//	req.body().age = 3;

	//	for (int i = 0; i < 4096; ++i)
	//	{
	//		req.body().name.append("h");
	//	}

	//	BOOST_CHECK(!req.to_binary(buffer));

	//	buffer.resize(1);

	//	BOOST_CHECK(!req.to_binary(buffer));

	//	buffer.resize(10);

	//	BOOST_CHECK(!req.to_binary(buffer));

	//	buffer.resize(12);

	//	req.header()->session_id = static_cast<uint32_t>(aquarius::uuid::invoke());

	//	BOOST_CHECK(!req.to_binary(buffer));
	//}

	{
		auto message = std::make_shared<aquarius::basic_message>();

		aquarius::flex_buffer_t buffer{};

		elastic::to_binary(13, buffer);
		
		for (int i = 0; i < 13; ++i)
		{
			elastic::to_binary(0, buffer);
		}

		BOOST_CHECK(buffer.size() == 14);

		BOOST_CHECK(message->from_binary(buffer));

		BOOST_CHECK(buffer.size() == 0);
	}

	{
		aquarius::flex_buffer_t buffer(0);

		person_request req{};

		BOOST_CHECK(!req.from_binary(buffer));
	}
}

BOOST_AUTO_TEST_CASE(process)
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

BOOST_AUTO_TEST_SUITE_END()