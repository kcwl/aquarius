#include "test_tcp.virgo.h"



login_req_body::login_req_body()
	: per_req()
{}

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

void login_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_resp, buffer);
}

void login_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	per_resp = this->parse_from<tcp_person>(buffer);
}

