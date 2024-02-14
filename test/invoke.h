#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius.hpp>

BOOST_AUTO_TEST_SUITE(invoke)

BOOST_AUTO_TEST_CASE(resolver)
{
	aquarius::flex_buffer_t buffer{};

	uint32_t proto{};

	std::size_t total{};

	auto result = aquarius::resolver<aquarius::tcp>::from_binay(buffer, proto, total);

	BOOST_CHECK(result == aquarius::read_handle_result::unknown_error);

	buffer.pubseekpos(0, std::ios::out);

	elastic::to_binary(10001u, buffer);

	result = aquarius::resolver<aquarius::tcp>::from_binay(buffer, proto, total);

	BOOST_CHECK(result == aquarius::read_handle_result::unknown_error);

	BOOST_CHECK(proto == 10001 && total == std::size_t{});

	buffer.pubseekpos(0, std::ios::out);

	elastic::to_binary(45u, buffer);

	result = aquarius::resolver<aquarius::tcp>::from_binay(buffer, proto, total);

	BOOST_CHECK(result == aquarius::read_handle_result::waiting_for_query);

	BOOST_CHECK(proto == 10001 && total == 45);

	buffer.pubseekpos(0, std::ios::out);

	buffer.commit(45);

	result = aquarius::resolver<aquarius::tcp>::from_binay(buffer, proto, total);

	BOOST_CHECK(result == aquarius::read_handle_result::ok);

	BOOST_CHECK(proto == 10001 && total == 45);
}

BOOST_AUTO_TEST_CASE(session)
{
	{
		aquarius::flex_buffer_t buffer{};

		auto result = aquarius::session_iovoke_helper::invoke(buffer, 10001);

		BOOST_CHECK(result == aquarius::read_handle_result::unknown_error);

		elastic::to_binary(12001u, buffer);

		auto session = std::make_shared<aquarius::session<aquarius::connect<aquarius::tcp, aquarius::conn_mode::server, aquarius::ssl_mode::ssl>>>(nullptr);

		aquarius::session_manager::instance().push(session);

		result = aquarius::session_iovoke_helper::invoke(buffer, session->uuid());

		BOOST_CHECK(result != aquarius::read_handle_result::ok);

		buffer.pubseekpos(0, std::ios::out);

		elastic::to_binary(45u, buffer);
		buffer.commit(45);

		result = aquarius::session_iovoke_helper::invoke(buffer, session->uuid());

		BOOST_CHECK(result != aquarius::read_handle_result::ok);
	}

	{
		aquarius::session_manager::instance().erase(0);
	}
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


BOOST_AUTO_TEST_CASE(visitor)
{
	auto req = std::make_shared<person_request>();

	aquarius::flex_buffer_t buffer{};

	req->to_binary(buffer);

	BOOST_CHECK(req->accept(buffer, nullptr, nullptr) == aquarius::read_handle_result::failed);

	auto msg = std::make_shared<aquarius::basic_message>();

	BOOST_CHECK(msg->accept(buffer, nullptr, nullptr) == aquarius::read_handle_result::failed);
}

BOOST_AUTO_TEST_SUITE_END()