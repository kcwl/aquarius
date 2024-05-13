#pragma once
#include "proto_regist.h"
#include <aquarius.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(basic_context)

BOOST_AUTO_TEST_CASE(basic_message_context)
{
	auto ctx = std::make_shared<aquarius::basic_context>();

	auto req = std::make_shared<aquarius::basic_message>();

	aquarius::flex_buffer_t buffer{};

	BOOST_CHECK(req->accept(buffer, ctx, nullptr));
}

BOOST_AUTO_TEST_CASE(call_back)
{
	{
		using connect_t = aquarius::ssl_tcp_server_connect;

		aquarius::asio::io_service io_service;

		aquarius::asio::ip::tcp::socket socket(io_service);

		using ssl_context_t = aquarius::asio::ssl_context_t;

		ssl_context_t ssl_context(ssl_context_t::sslv23);

		ssl_context.set_options(ssl_context_t::default_workarounds | ssl_context_t::no_sslv2 |
								ssl_context_t::single_dh_use);

		ssl_context.use_certificate_chain_file("crt/server.crt");
		ssl_context.use_private_key_file("crt/server.key", ssl_context_t::pem);
		ssl_context.use_tmp_dh_file("crt/dh512.pem");

		auto conn = std::make_shared<connect_t>(std::move(socket), ssl_context);

		auto session_ptr = std::make_shared<aquarius::session<connect_t>>(conn);

		aquarius::invoke_session_helper::push(session_ptr);

		session_ptr->attach(1300, std::make_shared<aquarius::basic_context>());
		session_ptr->attach(1300, std::make_shared<aquarius::basic_context>());
		session_ptr->attach(1301, std::make_shared<ctx_test_server>());
	}
}

BOOST_AUTO_TEST_CASE(function)
{
	auto request_ptr = std::make_shared<person_request>();

	aquarius::flex_buffer_t buffer{};

	request_ptr->to_binary(buffer);

	uint32_t proto{};
	elastic::from_binary(proto, buffer);

	auto context_ptr = std::dynamic_pointer_cast<aquarius::basic_context>(std::make_shared<ctx_test_server>());

	BOOST_CHECK(!request_ptr->accept(buffer, context_ptr, nullptr));
}

BOOST_AUTO_TEST_CASE(manager)
{
	{
		BOOST_CHECK(!aquarius::invoke_session_helper::push(nullptr));
	}

	{
		auto session = std::make_shared<aquarius::session<aquarius::ssl_tcp_server_connect>>(nullptr);

		aquarius::invoke_session_helper::push(session);

		aquarius::flex_buffer_t buffer{};

		BOOST_CHECK(!aquarius::message_router::process(buffer, session->uuid()));

		elastic::to_binary(aquarius::pack_flag::normal, buffer);

		elastic::to_binary(12001, buffer);

		BOOST_CHECK(!aquarius::message_router::process(buffer, session->uuid()));
	}

	{
		aquarius::flex_buffer_t buffer{};

		elastic::to_binary(aquarius::pack_flag::normal, buffer);

		BOOST_CHECK(!aquarius::message_router::process(buffer, 10001));

		elastic::to_binary(12001u, buffer);

		auto session = std::make_shared < aquarius::session<aquarius::ssl_tcp_server_connect>>(nullptr);

		elastic::to_binary(45u, buffer);
		buffer.commit(45);

		aquarius::message_router::process(buffer, session->uuid());

		BOOST_CHECK(aquarius::invoke_session_helper::broadcast(person_response{}));

		BOOST_CHECK(aquarius::invoke_session_helper::broadcast_if(person_response{}, [](auto) { return true; }));

		BOOST_CHECK(aquarius::invoke_session_helper::broadcast_if(person_response{}, [](auto) { return false; }));
	}

	{
		auto size = aquarius::invoke_session_helper::size();

		aquarius::invoke_session_helper::erase(0);

		BOOST_CHECK(aquarius::invoke_session_helper::size() == (size - 1));
	}
}

BOOST_AUTO_TEST_CASE(content)
{
	aquarius::tcp_server srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	person_request1 req{};
	req.body().age = 1;
	req.body().name = "world";

	cli.send_request(std::move(req), [&](std::shared_ptr<person_response1> resp) { BOOST_CHECK(true); });

	std::this_thread::sleep_for(1s);

	cli.stop();
	srv.stop();

	std::this_thread::sleep_for(1s);

	t.join();
	tc.join();
}

BOOST_AUTO_TEST_CASE(visitable)
{
	auto req = std::make_shared<person_request>();

	aquarius::flex_buffer_t buffer{};

	req->to_binary(buffer);

	std::size_t proto;

	elastic::from_binary(proto, buffer);

	BOOST_CHECK(!req->accept(buffer, nullptr, nullptr));

	auto msg = std::make_shared<aquarius::basic_message>();

	BOOST_CHECK(!msg->accept(buffer, nullptr, nullptr));
}

BOOST_AUTO_TEST_SUITE_END()