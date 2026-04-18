#define BOOST_TEST_NO_MAIN
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/scalar_type.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(serialize)

using namespace aquarius;

struct tcp_person
{
	bool sex;
	uint32 addr;
	int32 age;
	sint32 sage;
	uint64 telephone;
	int64 score;
	sint64 scores;
	fixed32 f;
	fixed64 g;
	bool aa;
	float hp;
	double mana;
	string name;
	bytes orders;
	std::vector<int32> ves;
};

BOOST_AUTO_TEST_CASE(binary)
{
	tcp_person p{};

	aquarius::binary_parse bp{};

	aquarius::flex_buffer buffer{};

	p.sex = true;
	p.addr = 2;
	p.age = 15;
	p.sage = -2;
	p.telephone = 15230214856;
	p.score = 100;
	p.scores = -3;
	p.f.value = 3;
	p.g.value = 4;
	p.hp = 200;
	p.mana = 300;
	p.name = "John";
	p.orders = { 1, 2, 3, 4, 5 };
	p.ves = { 2, 3, 4, 5 };

	bp.to_datas(p, buffer);

	tcp_person p1 = bp.from_datas<tcp_person>(buffer);

	BOOST_TEST(p1.sex == p.sex);
	BOOST_TEST(p1.addr == p.addr);
	BOOST_TEST(p1.age == p.age);
	BOOST_TEST(p1.sage == p.sage);
	BOOST_TEST(p1.telephone == p.telephone);
	BOOST_TEST(p1.score == p.score);
	BOOST_TEST(p1.scores == p.scores);
	BOOST_TEST(p1.f.value == p.f.value);
	BOOST_TEST(p1.g.value == p.g.value);
	BOOST_TEST(p1.aa == p.aa);
	BOOST_TEST(p1.hp == p.hp);
	BOOST_TEST(p1.mana == p.mana);
	BOOST_TEST(p1.name == p.name);
	BOOST_TEST(p1.orders == p.orders);
	BOOST_TEST(p1.ves == p.ves);
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