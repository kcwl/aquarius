#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include "proto_regist.h"
#include <aquarius.hpp>


BOOST_AUTO_TEST_SUITE(basic_context)

class ctx_timeout : public aquarius::context<timeout_request, timeout_response>
{
public:
	ctx_timeout()
		: aquarius::context<timeout_request, timeout_response>("ctx_timeout")
	{

	}

public:
	virtual aquarius::error_code handle()
	{
		std::this_thread::sleep_for(3s);

		send_response(0);

		return aquarius::errc::ok;
	}
};

class ctn_timeout : public aquarius::content<timeout_response>
{
public:
	ctn_timeout()
		: aquarius::content<timeout_response>("ctn_timeout")
	{

	}

public:
	virtual aquarius::error_code handle()
	{
		return aquarius::errc::ok;
	}
};

class ctx_execute_error : public aquarius::context<execute_error_request, execute_error_response>
{
public:
	ctx_execute_error()
		: aquarius::context<execute_error_request, execute_error_response>("ctx_execute_error")
	{

	}

public:
	virtual aquarius::error_code handle()
	{
		send_response(0);

		return aquarius::errc::unknown;
	}
};

class ctn_execute_error : public aquarius::content<execute_error_response>
{
public:
	ctn_execute_error()
		: aquarius::content<execute_error_response>("ctn_execute_error")
	{

	}

public:
	virtual aquarius::error_code handle()
	{
		return aquarius::errc::unknown;
	}
};

class ctx_client_timeout : public aquarius::context<client_timeout_request, client_timeout_response>
{
public:
	ctx_client_timeout()
		: aquarius::context<client_timeout_request, client_timeout_response>("ctx_client_timeout")
	{

	}

public:
	virtual aquarius::error_code handle()
	{
		send_response(0);

		return aquarius::errc::ok;
	}
};

class ctn_client_timeout : public aquarius::content<client_timeout_response>
{
public:
	ctn_client_timeout()
		: aquarius::content<client_timeout_response>("ctn_client_timeout")
	{

	}

public:
	virtual aquarius::error_code handle()
	{
		std::this_thread::sleep_for(3s);

		return aquarius::errc::ok;
	}
};

AQUARIUS_CONTEXT_REGIST(timeout_request, ctx_timeout);
AQUARIUS_CONTEXT_REGIST(timeout_response, ctn_timeout);
AQUARIUS_CONTEXT_REGIST(execute_error_request, ctx_execute_error);
AQUARIUS_CONTEXT_REGIST(execute_error_response, ctn_execute_error);
AQUARIUS_CONTEXT_REGIST(client_timeout_request, ctx_client_timeout);
AQUARIUS_CONTEXT_REGIST(client_timeout_response, ctn_client_timeout);

BOOST_AUTO_TEST_CASE(basic_message_context)
{
	auto ctx = std::make_shared<aquarius::basic_context>();

	auto req = std::make_shared<aquarius::basic_message>();

	aquarius::flex_buffer_t buffer{};

	BOOST_CHECK(req->accept(buffer, ctx, nullptr));
}

BOOST_AUTO_TEST_CASE(invoke)
{
	using connect_t = aquarius::ssl_tcp_server_connect;

	{
		std::size_t uid = 0;

		std::size_t conn_size = 0;

		{
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

			conn_size = aquarius::invoke_session_helper::size();

			aquarius::invoke_session_helper::push(conn);

			uid = conn->uuid();

			BOOST_CHECK(conn_size + 1 == aquarius::invoke_session_helper::size());
		}

		auto ptr = aquarius::invoke_session_helper::find(123);

		BOOST_CHECK(!ptr);

		ptr = aquarius::invoke_session_helper::find(uid);

		BOOST_CHECK(!ptr);

		aquarius::invoke_session_helper::erase(uid);

		BOOST_CHECK(conn_size == aquarius::invoke_session_helper::size());
	}
}
//
//BOOST_AUTO_TEST_CASE(function)
//{
//	auto request_ptr = std::make_shared<person_request>();
//
//	aquarius::flex_buffer_t buffer{};
//
//	aquarius::error_code ec{};
//	request_ptr->to_binary(buffer, ec);
//
//	uint32_t proto{};
//	elastic::from_binary(proto, buffer);
//
//	auto context_ptr = std::dynamic_pointer_cast<aquarius::basic_context>(std::make_shared<ctx_test_server>());
//
//	BOOST_CHECK(!request_ptr->accept(buffer, context_ptr, nullptr));
//}

//BOOST_AUTO_TEST_CASE(manager)
//{
//	{
//		BOOST_CHECK(!aquarius::invoke_session_helper::push(nullptr));
//	}
//
//	{
//		auto session = std::make_shared<aquarius::session<aquarius::ssl_tcp_server_connect>>(nullptr);
//
//		aquarius::invoke_session_helper::push(session);
//
//		aquarius::flex_buffer_t buffer{};
//
//		aquarius::packet pc{};
//
//		BOOST_CHECK(!pc.process(buffer, session->uuid()));
//
//		elastic::to_binary(aquarius::pack_flag::normal, buffer);
//
//		elastic::to_binary(12001, buffer);
//
//		BOOST_CHECK(!pc.process(buffer, session->uuid()));
//	}
//
//	{
//		aquarius::flex_buffer_t buffer{};
//
//		elastic::to_binary(aquarius::pack_flag::normal, buffer);
//
//		aquarius::packet pct{};
//
//		BOOST_CHECK(!pct.process(buffer, 10001));
//
//		elastic::to_binary(12001u, buffer);
//
//		auto session = std::make_shared<aquarius::session<aquarius::ssl_tcp_server_connect>>(nullptr);
//
//		elastic::to_binary(45u, buffer);
//		buffer.commit(45);
//
//		pct.process(buffer, session->uuid());
//
//		BOOST_CHECK(aquarius::invoke_session_helper::broadcast(person_response{}));
//
//		BOOST_CHECK(aquarius::invoke_session_helper::broadcast_if(person_response{}, [](auto) { return true; }));
//
//		BOOST_CHECK(aquarius::invoke_session_helper::broadcast_if(person_response{}, [](auto) { return false; }));
//	}
//
//	{
//		auto size = aquarius::invoke_session_helper::size();
//
//		aquarius::invoke_session_helper::erase(0);
//
//		BOOST_CHECK(aquarius::invoke_session_helper::size() == (size - 1));
//	}
//}

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

//BOOST_AUTO_TEST_CASE(visitable)
//{
//	auto req = std::make_shared<person_request>();
//
//	aquarius::flex_buffer_t buffer{};
//
//	aquarius::error_code ec{};
//
//
//	req->to_binary(buffer, ec);
//
//	std::size_t proto;
//
//	elastic::from_binary(proto, buffer);
//
//	BOOST_CHECK(!req->accept(buffer, nullptr, nullptr));
//
//	auto msg = std::make_shared<aquarius::basic_message>();
//
//	BOOST_CHECK(!msg->accept(buffer, nullptr, nullptr));
//}

BOOST_AUTO_TEST_CASE(timeout)
{
	{
		const char* ip_addr = "127.0.0.1";
		aquarius::no_ssl_tcp_server srv(8100, 5);
		std::thread srv_t([&] {srv.run(); });

		std::promise<bool> conn_result{};
		auto future = conn_result.get_future();

		aquarius::no_ssl_tcp_client cli(ip_addr, "8100", [&](auto result)
										{
											conn_result.set_value(result);
										});

		std::thread client_t([&] {cli.run(); });

		std::this_thread::sleep_for(1s);

		auto status = future.wait_for(5s);

		BOOST_CHECK(status == std::future_status::ready);

		timeout_request req{};
		req.body().age = 1;
		req.body().name = "person";

		cli.send_request(std::move(req), [&](std::shared_ptr<timeout_response> resp_ptr)
						 {
							 BOOST_CHECK(resp_ptr);
						 });

		std::this_thread::sleep_for(3s);

		srv.stop();
		cli.stop();

		srv_t.join();
		client_t.join();
	}

	{
		const char* ip_addr = "127.0.0.1";
		aquarius::no_ssl_tcp_server srv(8100, 5);
		std::thread srv_t([&] {srv.run(); });

		std::promise<bool> conn_result{};
		auto future = conn_result.get_future();

		aquarius::no_ssl_tcp_client cli(ip_addr, "8100", [&](auto result)
										{
											conn_result.set_value(result);
										});

		std::thread client_t([&] {cli.run(); });

		std::this_thread::sleep_for(1s);

		auto status = future.wait_for(5s);

		BOOST_CHECK(status == std::future_status::ready);

		client_timeout_request req{};
		req.body().age = 1;
		req.body().name = "person";

		cli.send_request(std::move(req), [&](std::shared_ptr<client_timeout_response> resp_ptr)
						 {
							 BOOST_CHECK(resp_ptr);
						 });

		std::this_thread::sleep_for(3s);

		srv.stop();
		cli.stop();

		srv_t.join();
		client_t.join();
	}
}

BOOST_AUTO_TEST_CASE(exe_error)
{
	const char* ip_addr = "127.0.0.1";
	aquarius::no_ssl_tcp_server srv(8100, 5);
	std::thread srv_t([&] {srv.run(); });

	std::promise<bool> conn_result{};
	auto future = conn_result.get_future();

	aquarius::no_ssl_tcp_client cli(ip_addr, "8100", [&](auto result)
									{
										conn_result.set_value(result);
									});

	std::thread client_t([&] {cli.run(); });

	std::this_thread::sleep_for(1s);

	auto status = future.wait_for(5s);

	BOOST_CHECK(status == std::future_status::ready);

	execute_error_request req{};
	req.body().age = 1;
	req.body().name = "person";

	cli.send_request(std::move(req), [&](std::shared_ptr<execute_error_response> resp_ptr)
					 {
						 BOOST_CHECK(resp_ptr);
					 });

	std::this_thread::sleep_for(3s);

	srv.stop();
	cli.stop();

	srv_t.join();
	client_t.join();
}

BOOST_AUTO_TEST_SUITE_END()