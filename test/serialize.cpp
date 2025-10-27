#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/detail/float.hpp>

BOOST_AUTO_TEST_SUITE(serialize)

struct person
{
	bool sex;
	uint32_t addr;
	int32_t age;
	uint64_t telephone;
	int64_t score;
	float hp;
	double mana;
	std::string name;
	std::vector<char> orders;
};

inline bool operator==(const person& lhs, const person& rhs)
{
	return lhs.sex == rhs.sex && lhs.addr == rhs.addr && lhs.age == rhs.age && lhs.telephone == rhs.telephone &&
		   lhs.score == rhs.score && aquarius::check_equal(lhs.hp, rhs.hp) &&
		   aquarius::check_equal(lhs.mana, rhs.mana) && lhs.name == rhs.name && lhs.orders == rhs.orders;
}

inline std::ostream& operator<<(std::ostream& os, const person& p)
{
	os << p.sex << "," << p.addr << "," << p.age << "," << p.telephone << "," << p.score << "," << p.hp << "," << p.mana
	   << ", [";

	for (auto& v : p.orders)
	{
		os << v << ",";
	}

	os.seekp(-1, std::ios::cur);

	os << "]\n";

	return os;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const person& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("sex", local.sex);
	jv_obj.emplace("addr", local.addr);
	jv_obj.emplace("age", local.age);
	jv_obj.emplace("telephone", local.telephone);
	jv_obj.emplace("score", local.score);
	jv_obj.emplace("hp", local.hp);
	jv_obj.emplace("mana", local.mana);
	jv_obj.emplace("name", local.name);

	aquarius::json::value vec{};
	auto& arr = vec.emplace_array();

	for (auto& l : local.orders)
	{
		arr.emplace_back(l);
	}

	jv_obj.emplace("orders", vec);
}

person tag_invoke(const aquarius::json::value_to_tag<person>&, const aquarius::json::value& jv)
{
	person result{};
	auto obj = jv.try_as_object();

	if (obj.has_error())
		return result;

	result.sex = (*obj).at("sex").as_bool();
	result.addr = static_cast<uint32_t>((*obj).at("addr").as_int64());
	result.age = static_cast<int32_t>((*obj).at("age").as_int64());
	result.telephone = (*obj).at("telephone").as_int64();
	result.score = (*obj).at("score").as_int64();
	result.hp = static_cast<float>((*obj).at("hp").as_double());
	result.mana = (*obj).at("mana").as_double();
	result.name = (*obj).at("name").as_string();

	auto arr = (*obj).at("orders").as_array();

	for (auto& a : arr)
	{
		result.orders.push_back(static_cast<char>(a.as_int64()));
	}

	return result;
}

BOOST_AUTO_TEST_CASE(binary)
{
	person p{};

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

	person p1 = bp.from_datas<person>(buffer);

	BOOST_CHECK_EQUAL(p, p1);
}

BOOST_AUTO_TEST_CASE(integer_limit)
{
	aquarius::binary_parse bp{};

	aquarius::flex_buffer buffer{};

	{
		constexpr int64_t i = std::numeric_limits<int64_t>::max();

		bp.to_datas(i, buffer);

		int64_t i1 = bp.from_datas<int64_t>(buffer);

		BOOST_CHECK_EQUAL(i, i1);
	}

	{
		constexpr int64_t i = std::numeric_limits<int64_t>::min();

		bp.to_datas(i, buffer);

		int64_t i1 = bp.from_datas<int64_t>(buffer);

		BOOST_CHECK_EQUAL(i, i1);
	}

	{
		constexpr uint64_t i = std::numeric_limits<uint64_t>::min();

		bp.to_datas(i, buffer);

		uint64_t i1 = bp.from_datas<uint64_t>(buffer);

		BOOST_CHECK_EQUAL(i, i1);
	}

	{
		constexpr uint64_t i = std::numeric_limits<uint64_t>::max();

		bp.to_datas(i, buffer);

		uint64_t i1 = bp.from_datas<uint64_t>(buffer);

		BOOST_CHECK_EQUAL(i, i1);
	}
}

BOOST_AUTO_TEST_CASE(json)
{
	aquarius::serialize::json_parse jp{};

	person p{};

	aquarius::flex_buffer buffer{};

	{
		auto p1 = jp.from_datas<person>(buffer);

		BOOST_CHECK_EQUAL(p1, p);
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

		BOOST_CHECK_EQUAL(p, p2);
	}
}
BOOST_AUTO_TEST_SUITE_END()