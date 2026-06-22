#include "cmd.virgo.h"

struct cmd_op_req_body::impl
{
	string command;
	std::vector<string> paramers;
};
cmd_op_req_body::cmd_op_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

cmd_op_req_body::~cmd_op_req_body(){}

void cmd_op_req_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<cmd_op_req_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->command, buffer,"command");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->paramers, buffer,"paramers");
	}
}

void cmd_op_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<cmd_op_req_body>(buffer); 
	}
	else
	{
		impl_ptr_->command = kv_base::parse_from<string>(buffer, "command");
		impl_ptr_->paramers = kv_base::parse_from<std::vector<string>>(buffer, "paramers");
	}
}
string cmd_op_req_body::command() const
{
	return impl_ptr_->command;
}
string& cmd_op_req_body::command()
{
	return impl_ptr_->command;
}
std::vector<string> cmd_op_req_body::paramers() const
{
	return impl_ptr_->paramers;
}
std::vector<string>& cmd_op_req_body::paramers()
{
	return impl_ptr_->paramers;
}
struct cmd_op_resp_body::impl
{
	string output;
};
cmd_op_resp_body::cmd_op_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

cmd_op_resp_body::~cmd_op_resp_body(){}

void cmd_op_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<cmd_op_resp_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->output, buffer,"output");
	}
}

void cmd_op_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<cmd_op_resp_body>(buffer); 
	}
	else
	{
		impl_ptr_->output = kv_base::parse_from<string>(buffer, "output");
	}
}
string cmd_op_resp_body::output() const
{
	return impl_ptr_->output;
}
string& cmd_op_resp_body::output()
{
	return impl_ptr_->output;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const cmd_op_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("command", local.impl_ptr_->command);
	jv_obj.emplace("paramers", aquarius::json_value_from_object<std::vector<string>>(local.impl_ptr_->paramers));
}

cmd_op_req_body tag_invoke(const aquarius::json::value_to_tag<cmd_op_req_body>&, const aquarius::json::value& jv)
{
	cmd_op_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->command = static_cast<string>(obj->at("command").as_string());
	result.impl_ptr_->paramers = aquarius::json::value_to<std::vector<string>>(obj->at("paramers"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const cmd_op_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("output", local.impl_ptr_->output);
}

cmd_op_resp_body tag_invoke(const aquarius::json::value_to_tag<cmd_op_resp_body>&, const aquarius::json::value& jv)
{
	cmd_op_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->output = static_cast<string>(obj->at("output").as_string());
	return result;
}
