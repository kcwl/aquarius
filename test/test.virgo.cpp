#include "test.virgo.h"

bool operator==(const login_req_header& lhs, const login_req_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}

std::ostream& operator<<(std::ostream& os, const login_req_header& other)
{
	return os << other.uuid;
}

void login_req_header::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_header_to(uuid, buffer);
}

void login_req_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	uuid = this->parse_header_from<int32_t>(buffer);
}

bool operator==(const login_req_body& lhs, const login_req_body& rhs)
{
	return lhs.per_req == rhs.per_req;
}

std::ostream& operator<<(std::ostream& os, const login_req_body& other)
{
	return os << other.per_req;
}

void login_req_body::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_body_to(per_req, buffer);
}

void login_req_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	per_req = this->parse_body_from<person>(buffer);
}

bool operator==(const login_resp_header& lhs, const login_resp_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}

std::ostream& operator<<(std::ostream& os, const login_resp_header& other)
{
	return os << other.uuid;
}

void login_resp_header::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_header_to(uuid, buffer);
}

void login_resp_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	uuid = this->parse_header_from<int32_t>(buffer);
}

bool operator==(const login_resp_body& lhs, const login_resp_body& rhs)
{
	return lhs.per_resp == rhs.per_resp;
}

std::ostream& operator<<(std::ostream& os, const login_resp_body& other)
{
	return os << other.per_resp;
}

void login_resp_body::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_body_to(per_resp, buffer);
}

void login_resp_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	per_resp = this->parse_body_from<person>(buffer);
}

