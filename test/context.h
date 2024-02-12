#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(basic_context)


BOOST_AUTO_TEST_CASE(error_message)
{
	auto ctx = std::make_shared<aquarius::basic_context>();

	auto req = std::make_shared<aquarius::basic_message>();

	aquarius::flex_buffer_t buffer{};

	auto result = req->accept(buffer, ctx, nullptr);

	ctx->on_accept();

	ctx->on_close();

	ctx->on_timeout();

	BOOST_CHECK(result == aquarius::read_handle_result::failed);
}

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
	virtual int handle() override
	{
		std::cout << "server ctx\n";
		response_.body().age = 1;
		response_.body().name = "hello";

		send_response(1);

		return 1;
	}
};

BOOST_AUTO_TEST_CASE(function)
{
	auto request_ptr = std::make_shared<person_request>();

	aquarius::flex_buffer_t buffer{};

	request_ptr->to_binary(buffer);

	auto context_ptr = std::dynamic_pointer_cast<aquarius::basic_context>(std::make_shared<ctx_test_server>());

	BOOST_CHECK(request_ptr->accept(buffer, context_ptr, nullptr) == aquarius::read_handle_result::ok);
}

BOOST_AUTO_TEST_SUITE_END()