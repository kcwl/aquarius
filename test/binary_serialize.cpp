#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include "test_tcp.virgo.h"

BOOST_AUTO_TEST_SUITE(serialize)

BOOST_AUTO_TEST_CASE(binary)
{
	tcp_person p{};

	aquarius::binary_parse bp{};

	aquarius::flex_buffer buffer{};

	p.sex = true;
	p.addr = 2;
	p.age = 15;
	p.telephone = 15230214856;
	p.score = 100;
	p.hp = 200;
	p.mana = 300;
	p.name = "John";
	p.orders = { 1, 2, 3, 4, 5 };

	bp.to_datas(p, buffer);

	tcp_person p1 = bp.from_datas<tcp_person>(buffer);

	BOOST_CHECK(p1 == p);
}

BOOST_AUTO_TEST_CASE(integer_limit)
{
	aquarius::binary_parse bp{};

	aquarius::flex_buffer buffer{};

	{
		constexpr int64_t i = std::numeric_limits<int64_t>::max();

		bp.to_datas(i, buffer);

		int64_t i1 = bp.from_datas<int64_t>(buffer);

		BOOST_TEST(i == i1);
	}

	{
		constexpr int64_t i = std::numeric_limits<int64_t>::min();

		bp.to_datas(i, buffer);

		int64_t i1 = bp.from_datas<int64_t>(buffer);

		BOOST_TEST(i == i1);
	}

	{
		constexpr uint64_t i = std::numeric_limits<uint64_t>::min();

		bp.to_datas(i, buffer);

		uint64_t i1 = bp.from_datas<uint64_t>(buffer);

		BOOST_TEST(i == i1);
	}

	{
		constexpr uint64_t i = std::numeric_limits<uint64_t>::max();

		bp.to_datas(i, buffer);

		uint64_t i1 = bp.from_datas<uint64_t>(buffer);

		BOOST_TEST(i == i1);
	}
}

BOOST_AUTO_TEST_SUITE_END()