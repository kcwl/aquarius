#include "test_http.virgo.h"


bool http_person::operator==(const http_person& other) const
{
	return sex == other.sex && addr == other.addr && age == other.age && telephone == other.telephone && score == other.score && hp == other.hp && mana == other.mana && name == other.name && orders == other.orders;
}

http_test_post_req_body::http_test_post_req_body()
	: uuid()
	, per_req()
{}

bool http_test_post_req_body::operator==(const http_test_post_req_body& other) const
{
	return uuid == other.uuid && per_req == other.per_req;
}

void http_test_post_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_post_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_post_req_body>(buffer); 
}
http_test_post_resp_body::http_test_post_resp_body()
	: uuid()
	, per_resp()
{}

bool http_test_post_resp_body::operator==(const http_test_post_resp_body& other) const
{
	return uuid == other.uuid && per_resp == other.per_resp;
}

void http_test_post_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_post_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_post_resp_body>(buffer); 
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
	this->parse_to(*this, buffer);
}

void http_test_get_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_get_resp_body>(buffer); 
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_person& local)
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

http_person tag_invoke(const aquarius::json::value_to_tag<http_person>&, const aquarius::json::value& jv)
{
	http_person result{};
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

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_post_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid);
	jv_obj.emplace("per_req", aquarius::json_value_from_object<http_person>(local.per_req));
}

http_test_post_req_body tag_invoke(const aquarius::json::value_to_tag<http_test_post_req_body>&, const aquarius::json::value& jv)
{
	http_test_post_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.per_req = aquarius::json::value_to<http_person>(obj->at("per_req"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_post_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid);
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<http_person>(local.per_resp));
}

http_test_post_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_post_resp_body>&, const aquarius::json::value& jv)
{
	http_test_post_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.per_resp = aquarius::json::value_to<http_person>(obj->at("per_resp"));
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
