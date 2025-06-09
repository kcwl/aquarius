#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <aquarius.hpp>
#include <aquarius_protocol.hpp>


struct person
{
	bool sex;
	uint32_t addr;
	int32_t age;
	uint64_t telephone;
	int64_t score;
	float hp;
	double mana;
	std::vector<uint8_t> info;
	std::string name;
	std::vector<int> orders;

	void swap(person& other)
	{
		std::swap(sex, other.sex);
		std::swap(addr, other.addr);
		std::swap(age, other.age);
		std::swap(telephone, other.telephone);
		std::swap(score, other.score);
		std::swap(hp, other.hp);
		std::swap(mana, other.mana);
		std::swap(info, other.info);
		std::swap(name, other.name);
		std::swap(orders, other.orders);
	}
};

template <>
struct aquarius::reflect<person>
{
	using value_type = person;

	constexpr static std::string_view topic()
	{
		return "person"sv;
	}

	constexpr static std::array<std::string_view, 10> fields()
	{
		return {
			"sex"sv, "addr"sv, "age"sv, "telephone"sv, "score"sv, "hp"sv, "mana"sv, "info"sv, "name"sv, "orders"sv
		};
	}
};

inline bool operator==(const person& lhs, const person& rhs)
{
	return lhs.sex == rhs.sex && lhs.addr == rhs.addr && lhs.age == rhs.age && lhs.telephone == rhs.telephone &&
		lhs.score == rhs.score && lhs.hp == rhs.hp && lhs.mana && rhs.mana && lhs.info == rhs.info &&
		lhs.name == rhs.name && lhs.orders == rhs.orders;
}

inline std::ostream& operator<<(std::ostream& os, const person& p)
{
	os << p.sex << "," << p.addr << "," << p.age << "," << p.telephone << "," << p.score << "," << p.hp << "," << p.mana
		<< ", [";

	for (auto& v : p.info)
	{
		os << v << ",";
	}

	os.seekp(-1, std::ios::cur);

	os << "]," << p.name << ", [";

	for (auto& v : p.info)
	{
		os << v << ",";
	}

	os.seekp(-1, std::ios::cur);

	os << "]\n";

	return os;
}

struct test
{
	using tcp_request = aquarius::ip::request<person, 1001>;
	using tcp_response = aquarius::ip::response<person, 1002>;
};


AQUARIUS_STREAM_HANDLER(ctx_test, test)
{
	response().body() = message()->body();

	co_return 0;
}
