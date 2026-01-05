#include "login.virgo.h"

login_req_body::login_req_body()
	: user()
	, passwd()
{}

bool login_req_body::operator==(const login_req_body& other) const
{
	return user == other.user && passwd == other.passwd;
}

void login_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(user, buffer);
	this->parse_to(passwd, buffer);
}

void login_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	user = this->parse_from<string>(buffer);
	passwd = this->parse_from<string>(buffer);
}

bool login_resp_body::operator==(const login_resp_body& other) const
{
	return true;
}

void login_resp_body::serialize(aquarius::flex_buffer& buffer)
{
}

void login_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user", local.user);
	jv_obj.emplace("passwd", local.passwd);
}

login_req_body tag_invoke(const aquarius::json::value_to_tag<login_req_body>&, const aquarius::json::value& jv)
{
	login_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.user = static_cast<string>(obj->at("user").as_string());
	result.passwd = static_cast<string>(obj->at("passwd").as_string());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
}

login_resp_body tag_invoke(const aquarius::json::value_to_tag<login_resp_body>&, const aquarius::json::value& jv)
{
	login_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	return result;
}

std::ostream& operator<<(std::ostream& os, const login_req_body& other)
{
	os << other.user<< other.passwd;
	return os;
}

std::ostream& operator<<(std::ostream& os, const login_resp_body& other)
{
	return os;
}
