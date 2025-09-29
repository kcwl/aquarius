#pragma once
#include <aquarius.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <string_view>
#include <aquarius/virgo/impl/tcp_custom_header.hpp>

using namespace std::string_view_literals;

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

using rpc_test_request = aquarius::virgo::tcp_request<"rpc_test", aquarius::virgo::custom_request_header, person>;
using rpc_test_response = aquarius::virgo::tcp_response<"rpc_test", aquarius::virgo::custom_response_header, person>;

AQUARIUS_SSL_TCP_HANDLER(rpc_test_request, rpc_test_response, ctx_test)
{
	response().body() = request()->body();

	co_return aquarius::error_code{};
}
