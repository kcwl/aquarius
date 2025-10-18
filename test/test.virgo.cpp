#include "test.virgo.h"


bool operator==(const person& lhs, const person& rhs)
{
	return lhs.sex == rhs.sex && lhs.addr == rhs.addr && lhs.age == rhs.age && lhs.telephone == rhs.telephone && lhs.score == rhs.score && lhs.hp == rhs.hp && lhs.mana == rhs.mana && lhs.name == rhs.name && lhs.orders == rhs.orders;
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
person tag_invoke(const aquarius::json::value_to_tag<person>&, const aquarius::json::value& jv)
{
	person result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.sex = static_cast<bool>(obj->at("sex").as_uint64());
	result.addr = static_cast<uint32>(obj->at("addr").as_uint64());
	result.age = static_cast<int32>(obj->at("age").as_uint64());
	result.telephone = static_cast<uint64>(obj->at("telephone").as_uint64());
	result.score = static_cast<int64>(obj->at("score").as_uint64());
	result.hp = static_cast<float>(obj->at("hp").as_uint64());
	result.mana = static_cast<double>(obj->at("mana").as_uint64());
	result.name = static_cast<string>(obj->at("name").as_string());
	result.orders = aquarius::serialize::json_value_to_array(obj->at("orders"));
	return result;
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
	jv_obj.emplace("orders", aquarius::serialize::json_value_from_array(local.orders)); 
}


void login_req_header::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_to(uuid, buffer);
}

void login_req_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	uuid = this->parse_from<int32>(buffer);
}


bool operator==(const login_req_header& lhs, const login_req_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}

std::ostream& operator<<(std::ostream& os, const login_req_header& other)
{
	os << other.uuid;
	return os;
}
login_req_header tag_invoke(const aquarius::json::value_to_tag<login_req_header>&, const aquarius::json::value& jv)
{
	login_req_header result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_uint64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_header& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
}


void login_req_body::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_to(per_req, buffer);
}

void login_req_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	per_req = this->parse_from<person>(buffer);
}


bool operator==(const login_req_body& lhs, const login_req_body& rhs)
{
	return lhs.per_req == rhs.per_req;
}

std::ostream& operator<<(std::ostream& os, const login_req_body& other)
{
	os << other.per_req;
	return os;
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

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_req", aquarius::serialize::json_value_from_object<person>(local.per_req)); 
}


void login_resp_header::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_to(uuid, buffer);
}

void login_resp_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	uuid = this->parse_from<int32>(buffer);
}


bool operator==(const login_resp_header& lhs, const login_resp_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}

std::ostream& operator<<(std::ostream& os, const login_resp_header& other)
{
	os << other.uuid;
	return os;
}
login_resp_header tag_invoke(const aquarius::json::value_to_tag<login_resp_header>&, const aquarius::json::value& jv)
{
	login_resp_header result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_uint64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_header& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
}


void login_resp_body::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_to(per_resp, buffer);
}

void login_resp_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	per_resp = this->parse_from<person>(buffer);
}


bool operator==(const login_resp_body& lhs, const login_resp_body& rhs)
{
	return lhs.per_resp == rhs.per_resp;
}

std::ostream& operator<<(std::ostream& os, const login_resp_body& other)
{
	os << other.per_resp;
	return os;
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

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_resp", aquarius::serialize::json_value_from_object<person>(local.per_resp)); 
}


void http_login_req_header::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_to(uuid, buffer);
}

void http_login_req_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	uuid = this->parse_from<int32>(buffer);
}


bool operator==(const http_login_req_header& lhs, const http_login_req_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}

std::ostream& operator<<(std::ostream& os, const http_login_req_header& other)
{
	os << other.uuid;
	return os;
}
http_login_req_header tag_invoke(const aquarius::json::value_to_tag<http_login_req_header>&, const aquarius::json::value& jv)
{
	http_login_req_header result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_uint64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_req_header& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
}


void http_login_req_body::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_to(per_req, buffer);
}

void http_login_req_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	per_req = this->parse_from<person>(buffer);
}


bool operator==(const http_login_req_body& lhs, const http_login_req_body& rhs)
{
	return lhs.per_req == rhs.per_req;
}

std::ostream& operator<<(std::ostream& os, const http_login_req_body& other)
{
	os << other.per_req;
	return os;
}
http_login_req_body tag_invoke(const aquarius::json::value_to_tag<http_login_req_body>&, const aquarius::json::value& jv)
{
	http_login_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.per_req = aquarius::json::value_to<person>(obj->at("per_req"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_req", aquarius::serialize::json_value_from_object<person>(local.per_req)); 
}


void http_login_resp_header::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_to(uuid, buffer);
}

void http_login_resp_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	uuid = this->parse_from<int32>(buffer);
}


bool operator==(const http_login_resp_header& lhs, const http_login_resp_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}

std::ostream& operator<<(std::ostream& os, const http_login_resp_header& other)
{
	os << other.uuid;
	return os;
}
http_login_resp_header tag_invoke(const aquarius::json::value_to_tag<http_login_resp_header>&, const aquarius::json::value& jv)
{
	http_login_resp_header result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_uint64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_resp_header& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
}


void http_login_resp_body::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_to(per_resp, buffer);
}

void http_login_resp_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	per_resp = this->parse_from<person>(buffer);
}


bool operator==(const http_login_resp_body& lhs, const http_login_resp_body& rhs)
{
	return lhs.per_resp == rhs.per_resp;
}

std::ostream& operator<<(std::ostream& os, const http_login_resp_body& other)
{
	os << other.per_resp;
	return os;
}
http_login_resp_body tag_invoke(const aquarius::json::value_to_tag<http_login_resp_body>&, const aquarius::json::value& jv)
{
	http_login_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_resp", aquarius::serialize::json_value_from_object<person>(local.per_resp)); 
}

