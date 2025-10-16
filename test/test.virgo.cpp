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

bool operator==(const http_login_req_header& lhs, const http_login_req_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}

std::ostream& operator<<(std::ostream& os, const http_login_req_header& other)
{
	return os << other.uuid;
}

void http_login_req_header::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	*this = this->parse_header_from<http_login_req_header>(buffer);
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_req_header& local)
{
	auto& jv_obj = jv.emplace_object();
	//jv_obj.emplace("per_req", local.per_req); 
}

http_login_req_header tag_invoke(const aquarius::json::value_to_tag<http_login_req_header>&, const aquarius::json::value& jv)
{
	http_login_req_header result{};
	//auto obj = jv.try_as_object();
	//if(obj->empty())
	//	return {};

	//auto param = obj->at("params").as_object();
	//if(param.empty())
	//	return {};

	//param.at("per_req");
	return result;
}

void http_login_req_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_header_to(*this, buffer);
}

bool operator==(const http_login_req_body& lhs, const http_login_req_body& rhs)
{
	return lhs.per_req == rhs.per_req;
}

std::ostream& operator<<(std::ostream& os, const http_login_req_body& other)
{
	return os << other.per_req;
}

void http_login_req_body::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	*this = this->parse_body_from<http_login_req_body>(buffer);
}

void http_login_req_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_body_to(*this, buffer);
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	//jv_obj.emplace("per_req", local.per_req); 
}

http_login_req_body tag_invoke(const aquarius::json::value_to_tag<http_login_req_body>&, const aquarius::json::value& jv)
{
	http_login_req_body result{};
	//auto obj = jv.try_as_object();
	//if(obj->empty())
	//	return {};

	//auto param = obj->at("params").as_object();
	//if(param.empty())
	//	return {};

	//param.at("per_req");
	return result;
}


bool operator==(const http_login_resp_header& lhs, const http_login_resp_header& rhs)
{
	return lhs.uuid == rhs.uuid;
}

std::ostream& operator<<(std::ostream& os, const http_login_resp_header& other)
{
	return os << other.uuid;
}

void http_login_resp_header::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	*this = this->parse_header_from<http_login_resp_header>(buffer);
}

void http_login_resp_header::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_header_to(*this, buffer);
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_resp_header& local)
{
	auto& jv_obj = jv.emplace_object();
	//jv_obj.emplace("per_req", local.per_req); 
}

http_login_resp_header tag_invoke(const aquarius::json::value_to_tag<http_login_resp_header>&, const aquarius::json::value& jv)
{
	http_login_resp_header result{};
	//auto obj = jv.try_as_object();
	//if(obj->empty())
	//	return {};

	//auto param = obj->at("params").as_object();
	//if(param.empty())
	//	return {};
	//result.per_resp = static_cast<person>(param.at("per_resp"));
	return result;
}


bool operator==(const http_login_resp_body& lhs, const http_login_resp_body& rhs)
{
	return lhs.per_resp == rhs.per_resp;
}

std::ostream& operator<<(std::ostream& os, const http_login_resp_body& other)
{
	return os << other.per_resp;
}

void http_login_resp_body::serialize(aquarius::detail::flex_buffer<char>& buffer)
{
	*this = this->parse_body_from<http_login_resp_body>(buffer);
}

void http_login_resp_body::deserialize(aquarius::detail::flex_buffer<char>& buffer)
{
	this->parse_body_to(*this, buffer);
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	//jv_obj.emplace("per_resp", local.per_resp); 
}

http_login_resp_body tag_invoke(const aquarius::json::value_to_tag<http_login_resp_body>&, const aquarius::json::value& jv)
{
	http_login_resp_body result{};
	//auto obj = jv.try_as_object();
	//if(obj->empty())
	//	return {};

	//auto param = obj->at("params").as_object();
	//if(param.empty())
	//	return {};
	//result.per_resp = static_cast<person>(param.at("per_resp"));
	return result;
}

