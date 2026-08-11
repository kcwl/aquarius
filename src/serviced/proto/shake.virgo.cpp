#include "shake.virgo.h"
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>

using namespace aquarius;

struct shake_req_body::impl
{
	string host;
	int32 port;
	string name;
	string group;
	sint32 weight;
	string version;
	std::vector<string> topics;
};
shake_req_body::shake_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

shake_req_body::~shake_req_body(){}

void shake_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->host, buffer);
	binary_parse::to_datas(impl_ptr_->port, buffer);
	binary_parse::to_datas(impl_ptr_->name, buffer);
	binary_parse::to_datas(impl_ptr_->group, buffer);
	binary_parse::to_datas(impl_ptr_->weight, buffer);
	binary_parse::to_datas(impl_ptr_->version, buffer);
	binary_parse::to_datas(impl_ptr_->topics, buffer);
}
void shake_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->host = binary_parse::from_datas<string>(buffer);
	impl_ptr_->port = binary_parse::from_datas<int32>(buffer);
	impl_ptr_->name = binary_parse::from_datas<string>(buffer);
	impl_ptr_->group = binary_parse::from_datas<string>(buffer);
	impl_ptr_->weight = binary_parse::from_datas<sint32>(buffer);
	impl_ptr_->version = binary_parse::from_datas<string>(buffer);
	impl_ptr_->topics = binary_parse::from_datas<std::vector<string>>(buffer);
}
void shake_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("host", impl_ptr_->host);
	jv_obj.emplace("port", impl_ptr_->port);
	jv_obj.emplace("name", impl_ptr_->name);
	jv_obj.emplace("group", impl_ptr_->group);
	jv_obj.emplace("weight", aquarius::json_value_from_object<sint32>(impl_ptr_->weight));
	jv_obj.emplace("version", impl_ptr_->version);
	jv_obj.emplace("topics", aquarius::json_value_from_object<std::vector<string>>(impl_ptr_->topics));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void shake_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->host = static_cast<string>(jv_obj->at("host").as_string());
	impl_ptr_->port = static_cast<int32>(jv_obj->at("port").as_int64());
	impl_ptr_->name = static_cast<string>(jv_obj->at("name").as_string());
	impl_ptr_->group = static_cast<string>(jv_obj->at("group").as_string());
	impl_ptr_->weight = aquarius::json::value_to<sint32>(jv_obj->at("weight"));
	impl_ptr_->version = static_cast<string>(jv_obj->at("version").as_string());
	impl_ptr_->topics = aquarius::json::value_to<std::vector<string>>(jv_obj->at("topics"));
}
void shake_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->host, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->port, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->name, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->group, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->weight, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->version, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->topics, buffer);
}
void shake_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->host = kv_parse::from_datas<string>(buffer, impl_ptr_->host);
	impl_ptr_->port = kv_parse::from_datas<int32>(buffer, impl_ptr_->port);
	impl_ptr_->name = kv_parse::from_datas<string>(buffer, impl_ptr_->name);
	impl_ptr_->group = kv_parse::from_datas<string>(buffer, impl_ptr_->group);
	impl_ptr_->weight = kv_parse::from_datas<sint32>(buffer, impl_ptr_->weight);
	impl_ptr_->version = kv_parse::from_datas<string>(buffer, impl_ptr_->version);
	impl_ptr_->topics = kv_parse::from_datas<std::vector<string>>(buffer, impl_ptr_->topics);
}
string shake_req_body::host() const
{
	return impl_ptr_->host;
}
string& shake_req_body::host()
{
	return impl_ptr_->host;
}
int32 shake_req_body::port() const
{
	return impl_ptr_->port;
}
int32& shake_req_body::port()
{
	return impl_ptr_->port;
}
string shake_req_body::name() const
{
	return impl_ptr_->name;
}
string& shake_req_body::name()
{
	return impl_ptr_->name;
}
string shake_req_body::group() const
{
	return impl_ptr_->group;
}
string& shake_req_body::group()
{
	return impl_ptr_->group;
}
sint32 shake_req_body::weight() const
{
	return impl_ptr_->weight;
}
sint32& shake_req_body::weight()
{
	return impl_ptr_->weight;
}
string shake_req_body::version() const
{
	return impl_ptr_->version;
}
string& shake_req_body::version()
{
	return impl_ptr_->version;
}
std::vector<string> shake_req_body::topics() const
{
	return impl_ptr_->topics;
}
std::vector<string>& shake_req_body::topics()
{
	return impl_ptr_->topics;
}
struct shake_resp_body::impl
{
	std::vector<string> topics;
};
shake_resp_body::shake_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

shake_resp_body::~shake_resp_body(){}

void shake_resp_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->topics, buffer);
}
void shake_resp_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->topics = binary_parse::from_datas<std::vector<string>>(buffer);
}
void shake_resp_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("topics", aquarius::json_value_from_object<std::vector<string>>(impl_ptr_->topics));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void shake_resp_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->topics = aquarius::json::value_to<std::vector<string>>(jv_obj->at("topics"));
}
void shake_resp_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->topics, buffer);
}
void shake_resp_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->topics = kv_parse::from_datas<std::vector<string>>(buffer, impl_ptr_->topics);
}
std::vector<string> shake_resp_body::topics() const
{
	return impl_ptr_->topics;
}
std::vector<string>& shake_resp_body::topics()
{
	return impl_ptr_->topics;
}
