#pragma once
#include <aquarius/request.hpp>
#include <aquarius/response.hpp>
#include <boost/test/unit_test_suite.hpp>

BOOST_AUTO_TEST_SUITE(message)

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
		ar & sex;
		ar & role_data;
		ar & mana;
		ar & hp;
		ar & age;
		ar & money;
		ar & name;
		ar & back_money;
		ar & crc;
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
		ar & sex;
		ar & role_data;
		ar & mana;
		ar & hp;
		ar & age;
		ar & money;
		ar & name;
		ar & back_money;
		ar & crc;
	}
};

using person_request = aquarius::request<person_body_request, 10000>;
using person_response = aquarius::response<person_body_response, 10001>;

BOOST_AUTO_TEST_CASE(construction)
{
	{
		person_request req{};

		req.body().age = 3;
		req.body().back_money = 4;

		person_request req1(std::move(req));

		BOOST_CHECK(req.header() == nullptr);

		BOOST_CHECK(req1.body().age == 3 && req.body().age == 0);

		BOOST_CHECK(req1.body().back_money == 4 && req.body().back_money == 0);
	}

	{
		person_request req{};

		req.body().age = 3;
		req.body().back_money = 4;

		person_request req1{};
		req1 = std::move(req);

		BOOST_CHECK(req.header() == nullptr);

		BOOST_CHECK(req1.body().age == 3 && req.body().age == 0);

		BOOST_CHECK(req1.body().back_money == 4 && req.body().back_money == 0);
	}

	{
		person_response resp{};

		resp.body().age = 3;
		resp.body().back_money = 4;

		person_response resp1(std::move(resp));

		BOOST_CHECK(resp.header() == nullptr);

		BOOST_CHECK(resp1.body().age == 3 && resp.body().age == 0);

		BOOST_CHECK(resp1.body().back_money == 4 && resp.body().back_money == 0);
	}

	{
		person_response resp{};

		resp.body().age = 3;
		resp.body().back_money = 4;

		person_response resp1{};
		resp1 = std::move(resp);

		BOOST_CHECK(resp.header() == nullptr);

		BOOST_CHECK(resp1.body().age == 3 && resp.body().age == 0);

		BOOST_CHECK(resp1.body().back_money == 4 && resp.body().back_money == 0);
	}
}

BOOST_AUTO_TEST_CASE(parse)
{
	{
		aquarius::flex_buffer_t buffer(1);

		person_request req{};
		req.body().age = 3;

		BOOST_CHECK(!req.to_binary(buffer));
	}

	{
		aquarius::flex_buffer_t buffer(2);

		person_request req{};

		BOOST_CHECK(!req.from_binary(buffer));
	}

	{
		aquarius::flex_buffer_t buffer{};
		elastic::to_binary(10001u, buffer);
		elastic::to_binary(3u, buffer);
		elastic::to_binary(1u, buffer);

		person_request req{};

		BOOST_CHECK(!req.from_binary(buffer));
	}

	//{
	//	aquarius::flex_buffer_t buffer{};

	//	person_request req{};

	//	aquarius::error_code ec{};

	//	req.to_binary(buffer, ec);

	//	person_request req1;

	//	uint32_t proto{};

	//	std::size_t total_size{};

	//	elastic::from_binary(proto, buffer);

	//	elastic::from_binary(total_size, buffer);

	//	req1.from_binary(buffer, ec);

	//	BOOST_CHECK(!ec);
	//}
}

BOOST_AUTO_TEST_SUITE_END()