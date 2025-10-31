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

		BOOST_TEST(p1 == p);
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

		BOOST_TEST(p == p2);
	}
}
BOOST_AUTO_TEST_SUITE_END()