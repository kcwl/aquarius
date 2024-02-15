#pragma once
#include <aquarius/context.hpp>
#include <aquarius/invoke.hpp>
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
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
	virtual int handle() override
	{
		response_.body().age = 1;

		send_response(1);

		return 1;
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

class test_service : public aquarius::service
{
public:
	test_service() = default;

public:
	virtual bool init() override
	{
		return true;
	}

	virtual bool config() override
	{
		return true;
	}

	virtual bool run() override
	{
		return true;
	}

	virtual void stop() override
	{
		return;
	}

	virtual bool enable() override
	{
		return true;
	}

	virtual std::string name() override
	{
		return "test_service";
	}
};

class test_service_1 : public aquarius::service
{
public:
	test_service_1() = default;

public:
	virtual bool init() override
	{
		return false;
	}

	virtual bool config() override
	{
		return true;
	}

	virtual bool run() override
	{
		return true;
	}

	virtual void stop() override
	{
		return;
	}

	virtual bool enable() override
	{
		return true;
	}

	virtual std::string name() override
	{
		return "test_service_1";
	}
};

class test_service_2 : public aquarius::service
{
public:
	test_service_2() = default;

public:
	virtual bool init() override
	{
		return true;
	}

	virtual bool config() override
	{
		return false;
	}

	virtual bool run() override
	{
		return true;
	}

	virtual void stop() override
	{
		return;
	}

	virtual bool enable() override
	{
		return true;
	}

	virtual std::string name() override
	{
		return "test_service_2";
	}
};

class test_service_3 : public aquarius::service
{
public:
	test_service_3() = default;

public:
	virtual bool init() override
	{
		return true;
	}

	virtual bool config() override
	{
		return true;
	}

	virtual bool run() override
	{
		return false;
	}

	virtual void stop() override
	{
		return;
	}

	virtual bool enable() override
	{
		return true;
	}

	virtual std::string name() override
	{
		return "test_service_3";
	}
};

class test_service_4 : public aquarius::service
{
public:
	test_service_4() = default;

public:
	virtual bool init() override
	{
		return true;
	}

	virtual bool config() override
	{
		return true;
	}

	virtual bool run() override
	{
		return true;
	}

	virtual void stop() override
	{
		return;
	}

	virtual bool enable() override
	{
		return false;
	}

	virtual std::string name() override
	{
		return "test_service_4";
	}
};

AQUARIUS_SERVICE_REGIST(10000, test_service);
AQUARIUS_SERVICE_REGIST(10001, test_service_1);
AQUARIUS_SERVICE_REGIST(10002, test_service_2);
AQUARIUS_SERVICE_REGIST(10003, test_service_3);
AQUARIUS_SERVICE_REGIST(10004, test_service_4);

BOOST_AUTO_TEST_CASE(invoke_service)
{
	auto res = aquarius::invoke_service_helper::run();

	BOOST_CHECK_EQUAL(res, false);

	res = aquarius::invoke_service_helper::stop_one(11010);

	BOOST_CHECK_EQUAL(res, false);

	res = aquarius::invoke_service_helper::restart_one(11010);

	BOOST_CHECK_EQUAL(res, false);

	res = aquarius::invoke_service_helper::stop_one(10000);

	BOOST_CHECK_EQUAL(res, true);

	res = aquarius::invoke_service_helper::restart_one(10000);

	BOOST_CHECK_EQUAL(res, true);
}

BOOST_AUTO_TEST_SUITE_END()