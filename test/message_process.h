#pragma once
#include <boost/test/unit_test_suite.hpp>
#include "../aquarius/include/aquarius.hpp"

BOOST_AUTO_TEST_SUITE(message_process)

struct test_req_body
{
	int a_;
	int b_;
	int c_;
};

struct test_resp_body
{
	int a_;
	int b_;
	int c_;
};

using test_request = aquarius::request<test_req_body, 1001>;
using test_response = aquarius::response<test_resp_body, 1002>;

class ctx_test : public aquarius::detail::context_impl<test_request, test_response>
{
public:
	ctx_test()
		: aquarius::detail::context_impl<test_request, test_response>("ctx_test")
	{}

public:
	virtual int handle() override
	{
		response_.body().a_ = request_ptr_->body().a_ + 1;
		response_.body().b_ = request_ptr_->body().b_ + 1;
		response_.body().c_ = request_ptr_->body().c_ + 1;

		send_response(1);

		return 1;
	}
};

CONTEXT_DEFINE(test_request, ctx_test);

class test_client : public aquarius::client
{
public:
	test_client(const std::string& ip, const std::string& port)
		: aquarius::client(ip, port)
	{}

public:
	virtual int read_handler() override
	{
		auto& buffer = this->get_recv_buffer();

		uint32_t proto = 0;

		elastic::from_binary(proto, buffer);

		resp_.visit(buffer, aquarius::visit_mode::input);

		result_.set_value(true);

		return 1;
	}

public:
	test_response resp_;

	std::promise<bool> result_;
};

MESSAGE_DEFINE(test_response);


BOOST_AUTO_TEST_CASE(process)
{
	aquarius::no_ssl_tcp_server srv("8100", 10);

	std::thread t([&] { srv.run(); });

	test_client cli("127.0.0.1", "8100");

	cli.run();

	test_request req{};
	req.body().a_ = 1;
	req.body().b_ = 2;
	req.body().c_ = 3;

	aquarius::flex_buffer_t buffer{};

	req.visit(buffer, aquarius::visit_mode::output);

	cli.async_write(std::move(buffer));

	cli.result_.get_future().wait();

	BOOST_CHECK_EQUAL(cli.resp_.body().a_, 2);
	BOOST_CHECK_EQUAL(cli.resp_.body().b_, 3);
	BOOST_CHECK_EQUAL(cli.resp_.body().c_, 4);

	cli.shut_down();

	srv.stop();
	t.join();
}

BOOST_AUTO_TEST_SUITE_END()