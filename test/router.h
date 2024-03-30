#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <memory>

BOOST_AUTO_TEST_SUITE(router)

struct Person
{
	int age;
};

using test_request = aquarius::request<Person, 1000>;
using test_response = aquarius::response<Person, 1001>;
using test_request_1 = aquarius::request<Person, 1002>;

class ctx_test_server : public aquarius::context<test_request, test_response>
{
public:
	ctx_test_server()
		: aquarius::context<test_request, test_response>("ctx_test_server")
	{}

public:
	virtual bool handle() override
	{
		response_.body().age = 1;

		send_response(1);

		return true;
	}
};

AQUARIUS_CONTEXT_REGIST(test_request, ctx_test_server);

BOOST_AUTO_TEST_CASE(invoke_message)
{
	auto msg_ptr = aquarius::invoke_message_helper::invoke(1000);

	auto req_ptr = std::dynamic_pointer_cast<test_request>(msg_ptr);

	BOOST_CHECK(req_ptr != nullptr);

	auto msg_ptr_1 = aquarius::invoke_message_helper::invoke(1001);

	auto req_ptr_1 = std::dynamic_pointer_cast<test_request_1>(msg_ptr_1);

	BOOST_CHECK(req_ptr_1 == nullptr);
}

BOOST_AUTO_TEST_CASE(invoke_context)
{
	auto ctx_ptr = aquarius::invoke_context_helper::invoke(1000);

	auto handle_ptr = std::dynamic_pointer_cast<ctx_test_server>(ctx_ptr);

	BOOST_CHECK(handle_ptr != nullptr);

	auto ctx_ptr_1 = aquarius::invoke_context_helper::invoke(1001);

	auto handle_ptr_1 = std::dynamic_pointer_cast<ctx_test_server>(ctx_ptr_1);

	BOOST_CHECK(handle_ptr_1 == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()