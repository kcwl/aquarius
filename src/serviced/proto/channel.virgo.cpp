#include "channel.virgo.h"

subscribe_req_body::subscribe_req_body()
	: topic()
	, payload()
{}

bool subscribe_req_body::operator==(const subscribe_req_body& other) const
{
	return topic == other.topic && payload == other.payload;
}

void subscribe_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(topic, buffer);
	this->parse_to(payload, buffer);
}

void subscribe_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	topic = this->parse_from<string>(buffer);
	payload = this->parse_from<string>(buffer);
}

bool subscribe_resp_body::operator==(const subscribe_resp_body& other) const
{
	return true;
}

void subscribe_resp_body::serialize(aquarius::flex_buffer& buffer)
{
}

void subscribe_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subscribe_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("topic", local.topic);
	jv_obj.emplace("payload", local.payload);
}

subscribe_req_body tag_invoke(const aquarius::json::value_to_tag<subscribe_req_body>&, const aquarius::json::value& jv)
{
	subscribe_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.topic = static_cast<string>(obj->at("topic").as_string());
	result.payload = static_cast<string>(obj->at("payload").as_string());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subscribe_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
}

subscribe_resp_body tag_invoke(const aquarius::json::value_to_tag<subscribe_resp_body>&, const aquarius::json::value& jv)
{
	subscribe_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	return result;
}

std::ostream& operator<<(std::ostream& os, const subscribe_req_body& other)
{
	os << other.topic<< other.payload;
	return os;
}

std::ostream& operator<<(std::ostream& os, const subscribe_resp_body& other)
{
	return os;
}
