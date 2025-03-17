#pragma once
#include "proto_regist.h"
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(message)

BOOST_AUTO_TEST_CASE(construction)
{
	{
		person_request req{};
		req.header()->set_session(1);
		req.body().age = 3;
		req.body().back_money = 4;
		req.add_length(2);

		person_request req1(std::move(req));

		BOOST_CHECK(req.header()->session() == 0);

		BOOST_CHECK(req1.body().age == 3 && req.body().age == 0);

		BOOST_CHECK(req1.body().back_money == 4 && req.body().back_money == 0);

		BOOST_CHECK(req.length() == 0);

		BOOST_CHECK(req.uuid() == 0);
	}

	{
		person_request req{};
		req.header()->set_session(1);
		req.body().age = 3;
		req.body().back_money = 4;
		req.add_length(2);

		person_request req1{};
		req1 = std::move(req);

		BOOST_CHECK(req.header()->session() == 0);

		BOOST_CHECK(req1.body().age == 3 && req.body().age == 0);

		BOOST_CHECK(req1.body().back_money == 4 && req.body().back_money == 0);

		BOOST_CHECK(req.length() == 0);

		BOOST_CHECK(req.uuid() == 0);
	}

	{
		person_response resp{};
		resp.header()->set_session(1);
		resp.header()->set_result( 2);
		resp.body().age = 3;
		resp.body().back_money = 4;

		person_response resp1(std::move(resp));

		BOOST_CHECK(resp.header()->session() == 0);

		BOOST_CHECK(resp.header()->result() == 0);

		BOOST_CHECK(resp1.body().age == 3 && resp.body().age == 0);

		BOOST_CHECK(resp1.body().back_money == 4 && resp.body().back_money == 0);

		BOOST_CHECK(resp.length() == 0);

		BOOST_CHECK(resp.uuid() == 0);
	}

	{
		person_response resp{};
		resp.header()->set_session(1);
		resp.header()->set_result(2);
		resp.body().age = 3;
		resp.body().back_money = 4;

		person_response resp1{};
		resp1 = std::move(resp);

		BOOST_CHECK(resp.header()->session() == 0);

		BOOST_CHECK(resp.header()->result() == 0);

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

		aquarius::to_binary(13, buffer);

		person_request req{};

		aquarius::error_code ec{};

		BOOST_CHECK(req.from_binary(buffer, ec));
	}

	{
		aquarius::flex_buffer_t buffer(0);

		person_request req{};

		aquarius::error_code ec{};

		BOOST_CHECK(req.from_binary(buffer, ec));
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

	cli.send_request(std::move(req), [&](std::shared_ptr<person_response> resp) { BOOST_CHECK(true); });

	std::this_thread::sleep_for(1s);

	cli.stop();
	srv.stop();

	std::this_thread::sleep_for(1s);

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_CASE(uid)
{
	aquarius::basic_message msg{};
	msg.set_uuid(1);

	BOOST_CHECK(msg.uuid() == 0);
}

BOOST_AUTO_TEST_SUITE_END()