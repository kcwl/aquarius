#pragma once
#include <aquarius.hpp>

enum color
{
	 red,
	 yellow,
	 green,
};

struct tcp_person
{
	bool operator==(const tcp_person & other) const; 

	bool sex;
	uint32 addr;
	int32 age;
	uint64 telephone;
	int64 score;
	float hp;
	double mana;
	string name;
	bytes orders;
};

class login_req_body : public aquarius::tcp_binary_serialize
{
public:
	login_req_body();
	virtual ~login_req_body() = default;

public:
	bool operator==(const login_req_body & other) const; 

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
	bool operator==(const login_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	tcp_person per_resp;
};


using login_request = aquarius::virgo::tcp_request<"10001", login_req_body>;
using login_response = aquarius::virgo::tcp_response<login_resp_body>;
