#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(invoke)


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

using person_request = aquarius::request<person_body_request, 10000>;
using person_response = aquarius::request<person_body_response, 10001>;


BOOST_AUTO_TEST_CASE(invoke_resolver_helper)
{
	aquarius::flex_buffer_t buffer{};

	uint32_t proto{};

	std::size_t total{};

	aquarius::error_code ec{};

	///aquarius::invoke_resolver_helper<aquarius::tcp>::from_binary(buffer, proto, total, ec);

	BOOST_CHECK(ec);

	buffer.pubseekpos(0, std::ios::out);

	elastic::to_binary(10001u, buffer);

	//aquarius::invoke_resolver_helper<aquarius::tcp>::from_binary(buffer, proto, total, ec);

	BOOST_CHECK(ec);

	BOOST_CHECK(proto == 10001 && total == std::size_t{});

	buffer.pubseekpos(0, std::ios::out);

	elastic::to_binary(45u, buffer);

	//aquarius::invoke_resolver_helper<aquarius::tcp>::from_binary(buffer, proto, total, ec);

	BOOST_CHECK(ec);

	BOOST_CHECK(proto == 10001 && total == 45);

	buffer.pubseekpos(0, std::ios::out);

	buffer.commit(45);

	//aquarius::invoke_resolver_helper<aquarius::tcp>::from_binary(buffer, proto, total, ec);

	BOOST_CHECK(!ec);

	BOOST_CHECK(proto == 10001 && total == 45);
}

BOOST_AUTO_TEST_CASE(session)
{
	{
		aquarius::flex_buffer_t buffer{};

		aquarius::error_code ec;

		BOOST_CHECK(aquarius::invoke_session_helper::process(buffer, 10001));

		elastic::to_binary(12001u, buffer);

		auto session = std::make_shared<aquarius::session<aquarius::connect<aquarius::tcp, aquarius::conn_mode::basic_server, aquarius::ssl_mode::ssl>>>(nullptr);

		aquarius::invoke_session_helper::push(session);

		BOOST_CHECK(aquarius::invoke_session_helper::process(buffer, session->uuid()));

		buffer.pubseekpos(0, std::ios::out);

		elastic::to_binary(45u, buffer);
		buffer.commit(45);

		aquarius::invoke_session_helper::process(buffer, session->uuid());

		aquarius::invoke_session_helper::broadcast(person_response{});

		aquarius::invoke_session_helper::broadcast_if(person_response{}, [](auto) {return true; });

		aquarius::invoke_session_helper::broadcast_if(person_response{}, [](auto) {return false; });
	}

	{
		aquarius::invoke_session_helper::erase(0);
	}

	{
		aquarius::invoke_session_helper::push(nullptr);
	}
}

BOOST_AUTO_TEST_CASE(visitor)
{
	auto req = std::make_shared<person_request>();

	aquarius::error_code ec{};

	aquarius::flex_buffer_t buffer{};

	req->to_binary(buffer, ec);

	BOOST_CHECK(req->accept(buffer, nullptr, nullptr));

	auto msg = std::make_shared<aquarius::basic_message>();

	BOOST_CHECK(msg->accept(buffer, nullptr, nullptr));
}

BOOST_AUTO_TEST_CASE(callback)
{
	{
		aquarius::invoke_callback_helper::regist(1, [](std::shared_ptr<person_request> ptr) {});

		std::shared_ptr<person_request> req;

		BOOST_CHECK(aquarius::invoke_callback_helper::apply(2, req) == false);
	}
	
	{
		aquarius::invoke_callback_helper::regist(1, [](std::shared_ptr<person_request> ptr) {});

		std::shared_ptr<person_response> resp;

		BOOST_CHECK(aquarius::invoke_callback_helper::apply(1, resp) == false);
	}
}

BOOST_AUTO_TEST_SUITE_END()