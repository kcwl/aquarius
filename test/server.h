#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>
//#include "test.pb.h"

BOOST_AUTO_TEST_SUITE(server)

using test_request = aquarius::request<Person, 10001>;
using test_response = aquarius::response<Person, 10002>;

class ctx_test_server : public aquarius::xhandle<test_request, test_response>
{
public:
	ctx_test_server()
		: aquarius::xhandle<test_request, test_response>("ctx_test_server")
	{}

public:
	virtual int handle() override
	{
		std::cout << "server ctx\n";
		response_.body().set_age(1);
		response_.body().set_name("hello");

		send_response(1);

		return 1;
	}
};

AQUARIUS_CONTEXT_REGIST(test_request, ctx_test_server);

class ctx_test_client : public aquarius::client_context<test_response>
{
public:
	ctx_test_client()
		: aquarius::client_context<test_response>("ctx_test_client")
	{}

public:
	virtual int handle() override
	{
		std::cout << "test response recved!\n";

		BOOST_CHECK_EQUAL(request_ptr_->body().age(), 1);
		BOOST_CHECK_EQUAL(request_ptr_->body().name(), "hello");

		return 1;
	}
};

AQUARIUS_CONTEXT_REGIST(test_response, ctx_test_client);

BOOST_AUTO_TEST_CASE(process_message)
{
	aquarius::tcp_server<0> srv(8100, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8100");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(2s);

	test_request req{};
	req.body().set_age(1);
	req.body().set_name("world");

	cli.async_write(std::move(req));

	std::this_thread::sleep_for(2s); 

	cli.stop();
	srv.stop();
	
	std::this_thread::sleep_for(2s);

	//t.join();
	//tc.join();
}

BOOST_AUTO_TEST_CASE(acceptor_error)
{
	aquarius::tcp_server<0> srv(8101, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8101");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(3s);

	cli.stop();
	srv.stop();
	//t.join();
	//tc.join();
}

BOOST_AUTO_TEST_CASE(connect_param)
{
	aquarius::tcp_server<0> srv(8102, 2);

	std::thread t([&] { srv.run(); });

	aquarius::tcp_client cli("127.0.0.1", "8102");

	std::thread tc([&] { cli.run(); });

	std::this_thread::sleep_for(1s);

	auto& connect_ptr = *srv.conns_.begin();

	if (connect_ptr)
	{
		BOOST_CHECK(connect_ptr->remote_address_u() ==2130706433);

		connect_ptr->set_rcvbuf_size(1024);

		BOOST_CHECK(connect_ptr->get_rcvbuf_size() == 1024);

		connect_ptr->set_sndbuf_size(2048);

		BOOST_CHECK(connect_ptr->get_sndbuf_size() == 2048);

		BOOST_CHECK(connect_ptr->keep_alive(true) == true);

		BOOST_CHECK(connect_ptr->set_nodelay(true) == true);

		BOOST_CHECK(connect_ptr->reuse_address(true) == true);

		BOOST_CHECK(connect_ptr->set_linger(true, 100) == true);


		connect_ptr->shut_down();

		BOOST_CHECK(connect_ptr->remote_address().empty() == true);

		BOOST_CHECK(connect_ptr->remote_address_u() == 0);

		BOOST_CHECK(connect_ptr->remote_port() == 0);
	}

	std::this_thread::sleep_for(1s);

	test_request req{};
	req.body().set_age(1);
	req.body().set_name("world");

	cli.async_write(std::move(req));

	std::this_thread::sleep_for(1s);

	srv.stop();
	cli.stop();

	//tc.join();
	//t.join();
}

BOOST_AUTO_TEST_SUITE_END()