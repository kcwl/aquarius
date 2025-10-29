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


void login_request_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(uuid, buffer);
	this->parse_to(per_resp, buffer);
}

void login_request_body::deserialize(aquarius::flex_buffer& buffer)
{
	uuid = this->parse_from<int32>(buffer);
	per_resp = this->parse_from<person>(buffer);
}

bool operator==(const login_request_body& lhs, const login_request_body& rhs)
{
	return lhs.uuid == rhs.uuid && lhs.per_resp == rhs.per_resp;
}


std::ostream& operator<<(std::ostream& os, const login_request_body& other)
{
	os << other.uuid<< other.per_resp;
	return os;
}
login_request_body tag_invoke(const aquarius::json::value_to_tag<login_request_body>&, const aquarius::json::value& jv)
{
	login_request_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_request_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<person>(local.per_resp)); 
}


void login_response_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(uuid, buffer);
	this->parse_to(per_resp, buffer);
}

void login_response_body::deserialize(aquarius::flex_buffer& buffer)
{
	uuid = this->parse_from<int32>(buffer);
	per_resp = this->parse_from<person>(buffer);
}

bool operator==(const login_response_body& lhs, const login_response_body& rhs)
{
	return lhs.uuid == rhs.uuid && lhs.per_resp == rhs.per_resp;
}


std::ostream& operator<<(std::ostream& os, const login_response_body& other)
{
	os << other.uuid<< other.per_resp;
	return os;
}
login_response_body tag_invoke(const aquarius::json::value_to_tag<login_response_body>&, const aquarius::json::value& jv)
{
	login_response_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_response_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<person>(local.per_resp)); 
}


void new_http_login_request_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void new_http_login_request_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<new_http_login_request_body>(buffer);
}

bool operator==(const new_http_login_request_body& lhs, const new_http_login_request_body& rhs)
{
	return lhs.uuid == rhs.uuid && lhs.per_resp == rhs.per_resp;
}


std::ostream& operator<<(std::ostream& os, const new_http_login_request_body& other)
{
	os << other.uuid<< other.per_resp;
	return os;
}
new_http_login_request_body tag_invoke(const aquarius::json::value_to_tag<new_http_login_request_body>&, const aquarius::json::value& jv)
{
	new_http_login_request_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const new_http_login_request_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<person>(local.per_resp)); 
}


void new_http_login_response_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void new_http_login_response_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<new_http_login_response_body>(buffer);
}

bool operator==(const new_http_login_response_body& lhs, const new_http_login_response_body& rhs)
{
	return lhs.uuid == rhs.uuid && lhs.per_resp == rhs.per_resp;
}


std::ostream& operator<<(std::ostream& os, const new_http_login_response_body& other)
{
	os << other.uuid<< other.per_resp;
	return os;
}
new_http_login_response_body tag_invoke(const aquarius::json::value_to_tag<new_http_login_response_body>&, const aquarius::json::value& jv)
{
	new_http_login_response_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const new_http_login_response_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<person>(local.per_resp)); 
}


void http_test_get_request_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(user, buffer, "user");
	this->parse_to(passwd, buffer, "passwd");
}

void http_test_get_request_body::deserialize(aquarius::flex_buffer& buffer)
{
	user = this->parse_from<int32>(buffer);
	passwd = this->parse_from<string>(buffer);
}

bool operator==(const http_test_get_request_body& lhs, const http_test_get_request_body& rhs)
{
	return lhs.user == rhs.user && lhs.passwd == rhs.passwd;
}


std::ostream& operator<<(std::ostream& os, const http_test_get_request_body& other)
{
	os << other.user<< other.passwd;
	return os;
}
http_test_get_request_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_request_body>&, const aquarius::json::value& jv)
{
	http_test_get_request_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.user = static_cast<int32>(obj->at("user").as_int64());
	result.passwd = static_cast<string>(obj->at("passwd").as_string());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_request_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user", local.user); 
	jv_obj.emplace("passwd", local.passwd); 
}


void http_test_get_response_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(user, buffer, "user");
	this->parse_to(passwd, buffer, "passwd");
}

void http_test_get_response_body::deserialize(aquarius::flex_buffer& buffer)
{
	user = this->parse_from<int32>(buffer);
	passwd = this->parse_from<string>(buffer);
}

bool operator==(const http_test_get_response_body& lhs, const http_test_get_response_body& rhs)
{
	return lhs.user == rhs.user && lhs.passwd == rhs.passwd;
}


std::ostream& operator<<(std::ostream& os, const http_test_get_response_body& other)
{
	os << other.user<< other.passwd;
	return os;
}
http_test_get_response_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_response_body>&, const aquarius::json::value& jv)
{
	http_test_get_response_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.user = static_cast<int32>(obj->at("user").as_int64());
	result.passwd = static_cast<string>(obj->at("passwd").as_string());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_response_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user", local.user); 
	jv_obj.emplace("passwd", local.passwd); 
}


void tcp_login_request_header::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(uuid, buffer);
}

void tcp_login_request_header::deserialize(aquarius::flex_buffer& buffer)
{
	uuid = this->parse_from<int32>(buffer);
}

void tcp_login_request_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_req, buffer);
}

void tcp_login_request_body::deserialize(aquarius::flex_buffer& buffer)
{
	per_req = this->parse_from<person>(buffer);
}

void tcp_login_response_header::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(uuid, buffer);
}

void tcp_login_response_header::deserialize(aquarius::flex_buffer& buffer)
{
	uuid = this->parse_from<int32>(buffer);
}

void tcp_login_response_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_resp, buffer);
}

void tcp_login_response_body::deserialize(aquarius::flex_buffer& buffer)
{
	per_resp = this->parse_from<person>(buffer);
}

bool operator==(const tcp_login_request_header& lhs, const tcp_login_request_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}


std::ostream& operator<<(std::ostream& os, const tcp_login_request_header& other)
{
	os << other.uuid;
	return os;
}

bool operator==(const tcp_login_request_body& lhs, const tcp_login_request_body& rhs)
{
	return lhs.per_req == rhs.per_req;
}


std::ostream& operator<<(std::ostream& os, const tcp_login_request_body& other)
{
	os << other.per_req;
	return os;
}

bool operator==(const tcp_login_response_header& lhs, const tcp_login_response_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}


std::ostream& operator<<(std::ostream& os, const tcp_login_response_header& other)
{
	os << other.uuid;
	return os;
}

bool operator==(const tcp_login_response_body& lhs, const tcp_login_response_body& rhs)
{
	return lhs.per_resp == rhs.per_resp;
}


std::ostream& operator<<(std::ostream& os, const tcp_login_response_body& other)
{
	os << other.per_resp;
	return os;
}
tcp_login_request_header tag_invoke(const aquarius::json::value_to_tag<tcp_login_request_header>&, const aquarius::json::value& jv)
{
	tcp_login_request_header result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const tcp_login_request_header& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
}

tcp_login_request_body tag_invoke(const aquarius::json::value_to_tag<tcp_login_request_body>&, const aquarius::json::value& jv)
{
	tcp_login_request_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.per_req = aquarius::json::value_to<person>(obj->at("per_req"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const tcp_login_request_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_req", aquarius::json_value_from_object<person>(local.per_req)); 
}

tcp_login_response_header tag_invoke(const aquarius::json::value_to_tag<tcp_login_response_header>&, const aquarius::json::value& jv)
{
	tcp_login_response_header result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const tcp_login_response_header& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
}

tcp_login_response_body tag_invoke(const aquarius::json::value_to_tag<tcp_login_response_body>&, const aquarius::json::value& jv)
{
	tcp_login_response_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const tcp_login_response_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<person>(local.per_resp)); 
}


void http_login_request_header::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_login_request_header::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_login_request_header>(buffer);
}

void http_login_request_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_login_request_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_login_request_body>(buffer);
}

void http_login_response_header::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_login_response_header::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_login_response_header>(buffer);
}

void http_login_response_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_login_response_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_login_response_body>(buffer);
}

bool operator==(const http_login_request_header& lhs, const http_login_request_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}


std::ostream& operator<<(std::ostream& os, const http_login_request_header& other)
{
	os << other.uuid;
	return os;
}

bool operator==(const http_login_request_body& lhs, const http_login_request_body& rhs)
{
	return lhs.per_req == rhs.per_req;
}


std::ostream& operator<<(std::ostream& os, const http_login_request_body& other)
{
	os << other.per_req;
	return os;
}

bool operator==(const http_login_response_header& lhs, const http_login_response_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}


std::ostream& operator<<(std::ostream& os, const http_login_response_header& other)
{
	os << other.uuid;
	return os;
}

bool operator==(const http_login_response_body& lhs, const http_login_response_body& rhs)
{
	return lhs.per_resp == rhs.per_resp;
}


std::ostream& operator<<(std::ostream& os, const http_login_response_body& other)
{
	os << other.per_resp;
	return os;
}
http_login_request_header tag_invoke(const aquarius::json::value_to_tag<http_login_request_header>&, const aquarius::json::value& jv)
{
	http_login_request_header result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_request_header& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
}

http_login_request_body tag_invoke(const aquarius::json::value_to_tag<http_login_request_body>&, const aquarius::json::value& jv)
{
	http_login_request_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.per_req = aquarius::json::value_to<person>(obj->at("per_req"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_request_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_req", aquarius::json_value_from_object<person>(local.per_req)); 
}

http_login_response_header tag_invoke(const aquarius::json::value_to_tag<http_login_response_header>&, const aquarius::json::value& jv)
{
	http_login_response_header result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.uuid = static_cast<int32>(obj->at("uuid").as_int64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_response_header& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.uuid); 
}

http_login_response_body tag_invoke(const aquarius::json::value_to_tag<http_login_response_body>&, const aquarius::json::value& jv)
{
	http_login_response_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};

	result.per_resp = aquarius::json::value_to<person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_response_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<person>(local.per_resp)); 
}

