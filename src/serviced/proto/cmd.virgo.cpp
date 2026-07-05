#include "cmd.virgo.h"
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>

using namespace aquarius;

struct cmd_op_req_body::impl
{
	string command;
	std::vector<string> paramers;
};
cmd_op_req_body::cmd_op_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

cmd_op_req_body::~cmd_op_req_body(){}

void cmd_op_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->command, buffer);
	binary_parse::to_datas(impl_ptr_->paramers, buffer);
}
void cmd_op_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->command = binary_parse::from_datas<string>(buffer);
	impl_ptr_->paramers = binary_parse::from_datas<std::vector<string>>(buffer);
}
void cmd_op_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("command", impl_ptr_->command);
	jv_obj.emplace("paramers", aquarius::json_value_from_object<std::vector<string>>(impl_ptr_->paramers));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void cmd_op_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->command = static_cast<string>(jv_obj->at("command").as_string());
	impl_ptr_->paramers = aquarius::json::value_to<std::vector<string>>(jv_obj->at("paramers"));
}
void cmd_op_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->command, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->paramers, buffer);
}
void cmd_op_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->command = kv_parse::from_datas<string>(buffer, impl_ptr_->command);
	impl_ptr_->paramers = kv_parse::from_datas<std::vector<string>>(buffer, impl_ptr_->paramers);
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

void cmd_op_resp_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->output, buffer);
}
void cmd_op_resp_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->output = binary_parse::from_datas<string>(buffer);
}
void cmd_op_resp_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("output", impl_ptr_->output);
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void cmd_op_resp_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->output = static_cast<string>(jv_obj->at("output").as_string());
}
void cmd_op_resp_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->output, buffer);
}
void cmd_op_resp_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->output = kv_parse::from_datas<string>(buffer, impl_ptr_->output);
}
string cmd_op_resp_body::output() const
{
	return impl_ptr_->output;
}
string& cmd_op_resp_body::output()
{
	return impl_ptr_->output;
}
