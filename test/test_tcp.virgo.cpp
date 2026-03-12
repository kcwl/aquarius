#include "test_tcp.virgo.h"


bool tcp_person::operator==(const tcp_person& other) const
{
	return sex == other.sex && addr == other.addr && age == other.age && telephone == other.telephone && score == other.score && hp == other.hp && mana == other.mana && name == other.name && orders == other.orders;
}

login_req_body::login_req_body()
	: per_req()
{}

bool login_req_body::operator==(const login_req_body& other) const
{
	return per_req == other.per_req;
}

void login_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_req, buffer);
}

void login_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	per_req = this->parse_from<tcp_person>(buffer);
}
login_resp_body::login_resp_body()
	: per_resp()
{}

bool login_resp_body::operator==(const login_resp_body& other) const
{
	return per_resp == other.per_resp;
}

void login_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_resp, buffer);
}

void login_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	per_resp = this->parse_from<tcp_person>(buffer);
}
