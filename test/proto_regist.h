#pragma once

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

	void swap(person_body_request& other)
	{
		std::swap(sex, other.sex);
		std::swap(role_data, other.role_data);
		std::swap(mana, other.mana);
		std::swap(hp, other.hp);
		std::swap(age, other.age);
		std::swap(money, other.money);
		std::swap(name, other.name);
		std::swap(back_money, other.back_money);
		std::swap(crc, other.crc);
	}

private:
	friend class elastic::access;

	template <typename _Archive>
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

	void swap(person_body_response& other)
	{
		std::swap(sex, other.sex);
		std::swap(role_data, other.role_data);
		std::swap(mana, other.mana);
		std::swap(hp, other.hp);
		std::swap(age, other.age);
		std::swap(money, other.money);
		std::swap(name, other.name);
		std::swap(back_money, other.back_money);
		std::swap(crc, other.crc);
	}

private:
	friend class elastic::access;

	template <typename _Archive>
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
		response_.body().age = 1;
		response_.body().name = "hello";

		send_response(0);

		return aquarius::errc::unknown;
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
		BOOST_CHECK_EQUAL(response_ptr_->body().age, 1);
		BOOST_CHECK_EQUAL(response_ptr_->body().name, "hello");

		return aquarius::errc::ok;
	}
};

AQUARIUS_CONTEXT_REGIST(person_response, ctx_test_client);


using person_request1 = aquarius::request<person_body_request, 10002>;
using person_response1 = aquarius::response<person_body_response, 10003>;

class ctx_test_server1 : public aquarius::context<person_request1, person_response1>
{
public:
	ctx_test_server1()
		: aquarius::context<person_request1, person_response1>("ctx_test_server")
	{}

public:
	virtual aquarius::error_code handle() override
	{
		response_.body().age = 1;
		response_.body().name = "hello";

		send_response(0);

		return aquarius::errc::unknown;
	}
};

AQUARIUS_CONTEXT_REGIST(person_request1, ctx_test_server1);

class ctx_test_client1 : public aquarius::content<person_response1>
{
public:
	ctx_test_client1()
		: aquarius::content<person_response1>("ctx_test_client")
	{}

public:
	virtual aquarius::error_code handle() override
	{
		BOOST_CHECK_EQUAL(response_ptr_->body().age, 1);
		BOOST_CHECK_EQUAL(response_ptr_->body().name, "hello");

		return aquarius::errc::unknown;
	}
};

AQUARIUS_CONTEXT_REGIST(person_response1, ctx_test_client1);