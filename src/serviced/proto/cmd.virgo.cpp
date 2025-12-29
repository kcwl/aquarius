#include "cmd.virgo.h"

cmd_op_req_body::cmd_op_req_body()
	: input()
{}

bool cmd_op_req_body::operator==(const cmd_op_req_body& other) const
{
	return input == other.input;
}

void cmd_op_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(input, buffer);
}

void cmd_op_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	input = this->parse_from<bytes>(buffer);
}
cmd_op_resp_body::cmd_op_resp_body()
	: output()
{}

bool cmd_op_resp_body::operator==(const cmd_op_resp_body& other) const
{
	return output == other.output;
}

void cmd_op_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(output, buffer);
}

void cmd_op_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	output = this->parse_from<string>(buffer);
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const cmd_op_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("input", aquarius::json_value_from_array(local.input));
}

cmd_op_req_body tag_invoke(const aquarius::json::value_to_tag<cmd_op_req_body>&, const aquarius::json::value& jv)
{
	cmd_op_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.input = aquarius::json_value_to_array(obj->at("input"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const cmd_op_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("output", local.output);
}

cmd_op_resp_body tag_invoke(const aquarius::json::value_to_tag<cmd_op_resp_body>&, const aquarius::json::value& jv)
{
	cmd_op_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.output = static_cast<string>(obj->at("output").as_string());
	return result;
}

std::ostream& operator<<(std::ostream& os, const cmd_op_req_body& other)
{
	os << "[";
	for (auto& s : other.input)
	{
		os << s << ", ";
	}
	os.seekp(-2, std::ios::cur);
	os << "]";
;
	return os;
}

std::ostream& operator<<(std::ostream& os, const cmd_op_resp_body& other)
{
	os << other.output;
	return os;
}
