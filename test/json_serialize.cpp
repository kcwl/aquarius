#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/json.hpp>
#include "test.virgo.h"

BOOST_AUTO_TEST_SUITE(serialize)

BOOST_AUTO_TEST_CASE(json)
{
	aquarius::json_parse jp{};

	person p{};

	aquarius::flex_buffer buffer{};

	{
		auto p1 = jp.from_datas<person>(buffer);

		BOOST_TEST(p1.sex == p.sex);
		BOOST_TEST(p1.addr == p.addr);
		BOOST_TEST(p1.age == p.age);
		BOOST_TEST(p1.telephone == p.telephone);
		BOOST_TEST(p1.score == p.score);
		BOOST_TEST(p1.hp == p.hp);
		BOOST_TEST(p1.mana == p.mana);
		BOOST_TEST(p1.name == p.name);
		BOOST_TEST(p1.orders == p.orders);
	}

	{
		p.sex = true;
		p.addr = 2;
		p.age = 15;
		p.telephone = 15230214856;
		p.score = 100;
		p.hp = 200;
		p.mana = 300;
		p.name = "John";
		p.orders = { 1, 2, 3, 4, 5 };

		jp.to_datas(p, buffer);

		person p2 = jp.from_datas<person>(buffer);

		BOOST_TEST(p2.sex == p.sex);
		BOOST_TEST(p2.addr == p.addr);
		BOOST_TEST(p2.age == p.age);
		BOOST_TEST(p2.telephone == p.telephone);
		BOOST_TEST(p2.score == p.score);
		BOOST_TEST(p2.hp == p.hp);
		BOOST_TEST(p2.mana == p.mana);
		BOOST_TEST(p2.name == p.name);
		BOOST_TEST(p2.orders == p.orders);
	}
}
BOOST_AUTO_TEST_SUITE_END()