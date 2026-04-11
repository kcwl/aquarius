#pragma once
#include <aquarius.hpp>
using namespace aquarius;

enum color
{
	 red,
	 yellow,
	 green,
};

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
	std::map<int32, int32> seqs;
	std::vector<int32> ves;
};

class login_req_body : public aquarius::tcp_binary_serialize
{
public:
	login_req_body();
	virtual ~login_req_body() = default;


public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	tcp_person per_req;
};
class login_resp_body : public aquarius::tcp_binary_serialize
{
public:
	login_resp_body();
	virtual ~login_resp_body() = default;


public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	tcp_person per_resp;
};

struct test_table
{
	int64 id;
};

using login_request = aquarius::tcp_request<"10001", login_req_body>;
using login_response = aquarius::tcp_response<login_resp_body>;
