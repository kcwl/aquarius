#include "test.virgo.h"


bool person::operator==(const person& other) const
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
	per_req = this->parse_from<person>(buffer);
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
	per_resp = this->parse_from<person>(buffer);
}

new_http_login_req_body::new_http_login_req_body()
	: uuid()
	, per_req()
{}

bool new_http_login_req_body::operator==(const new_http_login_req_body& other) const
{
	return uuid == other.uuid && per_req == other.per_req;
}

void new_http_login_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void new_http_login_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<new_http_login_req_body>(buffer); 
}
new_http_login_resp_body::new_http_login_resp_body()
	: uuid()
	, per_resp()
{}

bool new_http_login_resp_body::operator==(const new_http_login_resp_body& other) const
{
	return uuid == other.uuid && per_resp == other.per_resp;
}

void new_http_login_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void new_http_login_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<new_http_login_resp_body>(buffer); 
}

http_test_get_req_body::http_test_get_req_body()
	: user()
	, passwd()
{}

bool http_test_get_req_body::operator==(const http_test_get_req_body& other) const
{
	return user == other.user && passwd == other.passwd;
}

void http_test_get_req_body::serialize(aquarius::flex_buffer& buffer)
{
	buffer.sputc('?');
	this->parse_to(user, buffer, "user");
	buffer.sputc('&');
	this->parse_to(passwd, buffer, "passwd");
}

void http_test_get_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	user = this->parse_from<int32>(buffer, "user");
	passwd = this->parse_from<string>(buffer, "passwd");
}
http_test_get_resp_body::http_test_get_resp_body()
	: user()
	, passwd()
{}

bool http_test_get_resp_body::operator==(const http_test_get_resp_body& other) const
{
	return user == other.user && passwd == other.passwd;
}

void http_test_get_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	buffer.sputc('?');
	this->parse_to(user, buffer, "user");
	buffer.sputc('&');
	this->parse_to(passwd, buffer, "passwd");
}

void http_test_get_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	user = this->parse_from<int32>(buffer, "user");
	passwd = this->parse_from<string>(buffer, "passwd");
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
	jv_obj.emplace("orders", aquarius::json_value_from_array(local.orders));
}

person tag_invoke(const aquarius::json::value_to_tag<person>&, const aquarius::json::value& jv)
{
	person result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.sex = obj->at("sex").as_bool();
	result.addr = static_cast<uint32>(obj->at("addr").as_int64());
	result.age = static_cast<int32>(obj->at("age").as_int64());
	result.telephone = static_cast<uint64>(obj->at("telephone").as_int64());
	result.score = obj->at("score").as_int64();
	result.hp = static_cast<float>(obj->at("hp").as_double());
	result.mana = obj->at("mana").as_double();
	result.name = static_cast<string>(obj->at("name").as_string());
	result.orders = aquarius::json_value_to_array(obj->at("orders"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_req", aquarius::json_value_from_object<person>(local.per_req));
}

login_req_body tag_invoke(const aquarius::json::value_to_tag<login_req_body>&, const aquarius::json::value& jv)
{
	login_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.per_req = aquarius::json::value_to<person>(obj->at("per_req"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<person>(local.per_resp));
}

login_resp_body tag_invoke(const aquarius::json::value_to_tag<login_resp_body>&, const aquarius::json::value& jv)
{
	login_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const new_http_login_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid);
	jv_obj.emplace("per_req", aquarius::json_value_from_object<person>(local.per_req));
}

new_http_login_req_body tag_invoke(const aquarius::json::value_to_tag<new_http_login_req_body>&, const aquarius::json::value& jv)
{
	new_http_login_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.per_req = aquarius::json::value_to<person>(obj->at("per_req"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const new_http_login_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid);
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<person>(local.per_resp));
}

new_http_login_resp_body tag_invoke(const aquarius::json::value_to_tag<new_http_login_resp_body>&, const aquarius::json::value& jv)
{
	new_http_login_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user", local.user);
	jv_obj.emplace("passwd", local.passwd);
}

http_test_get_req_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_req_body>&, const aquarius::json::value& jv)
{
	http_test_get_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.user = static_cast<int32>(obj->at("user").as_int64());
	result.passwd = static_cast<string>(obj->at("passwd").as_string());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user", local.user);
	jv_obj.emplace("passwd", local.passwd);
}

http_test_get_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_resp_body>&, const aquarius::json::value& jv)
{
	http_test_get_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.user = static_cast<int32>(obj->at("user").as_int64());
	result.passwd = static_cast<string>(obj->at("passwd").as_string());
	return result;
}

std::ostream& operator<<(std::ostream& os, const person& other)
{
	os << other.sex<< other.addr<< other.age<< other.telephone<< other.score<< other.hp<< other.mana<< other.name<< "[";
	for (auto& s : other.orders)
	{
		os << s << ", ";
	}
	os.seekp(-2, std::ios::cur);
	os << "]";
;
	return os;
}

std::ostream& operator<<(std::ostream& os, const login_req_body& other)
{
	os << other.per_req;
	return os;
}

std::ostream& operator<<(std::ostream& os, const login_resp_body& other)
{
	os << other.per_resp;
	return os;
}

std::ostream& operator<<(std::ostream& os, const new_http_login_req_body& other)
{
	os << other.uuid<< other.per_req;
	return os;
}

std::ostream& operator<<(std::ostream& os, const new_http_login_resp_body& other)
{
	os << other.uuid<< other.per_resp;
	return os;
}

std::ostream& operator<<(std::ostream& os, const http_test_get_req_body& other)
{
	os << other.user<< other.passwd;
	return os;
}

std::ostream& operator<<(std::ostream& os, const http_test_get_resp_body& other)
{
	os << other.user<< other.passwd;
	return os;
}
