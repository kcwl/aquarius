#include "regist.virgo.h"

regist_req_body::regist_req_body()
	: protocol()
	, ip_addr()
	, port()
{}

bool regist_req_body::operator==(const regist_req_body& other) const
{
	return protocol == other.protocol && ip_addr == other.ip_addr && port == other.port;
}

void regist_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(protocol, buffer);
	this->parse_to(ip_addr, buffer);
	this->parse_to(port, buffer);
}

void regist_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	protocol = this->parse_from<int32>(buffer);
	ip_addr = this->parse_from<string>(buffer);
	port = this->parse_from<int32>(buffer);
}

bool regist_resp_body::operator==(const regist_resp_body& other) const
{
	return true;
}

void regist_resp_body::serialize(aquarius::flex_buffer& buffer)
{
}

void regist_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const regist_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("protocol", local.protocol);
	jv_obj.emplace("ip_addr", local.ip_addr);
	jv_obj.emplace("port", local.port);
}

regist_req_body tag_invoke(const aquarius::json::value_to_tag<regist_req_body>&, const aquarius::json::value& jv)
{
	regist_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.protocol = static_cast<int32>(obj->at("protocol").as_int64());
	result.ip_addr = static_cast<string>(obj->at("ip_addr").as_string());
	result.port = static_cast<int32>(obj->at("port").as_int64());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const regist_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
}

regist_resp_body tag_invoke(const aquarius::json::value_to_tag<regist_resp_body>&, const aquarius::json::value& jv)
{
	regist_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	return result;
}

std::ostream& operator<<(std::ostream& os, const regist_req_body& other)
{
	os << other.protocol<< other.ip_addr<< other.port;
	return os;
}

std::ostream& operator<<(std::ostream& os, const regist_resp_body& other)
{
	return os;
}
