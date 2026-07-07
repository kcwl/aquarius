#include "regist.virgo.h"
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>

using namespace aquarius;

struct regist_req_body::impl
{
	string name;
	string host;
	int32 port;
	bool healthy;
	string group;
	int32 weight;
	string version;
};
regist_req_body::regist_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

regist_req_body::~regist_req_body(){}

void regist_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->name, buffer);
	binary_parse::to_datas(impl_ptr_->host, buffer);
	binary_parse::to_datas(impl_ptr_->port, buffer);
	binary_parse::to_datas(impl_ptr_->healthy, buffer);
	binary_parse::to_datas(impl_ptr_->group, buffer);
	binary_parse::to_datas(impl_ptr_->weight, buffer);
	binary_parse::to_datas(impl_ptr_->version, buffer);
}
void regist_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->name = binary_parse::from_datas<string>(buffer);
	impl_ptr_->host = binary_parse::from_datas<string>(buffer);
	impl_ptr_->port = binary_parse::from_datas<int32>(buffer);
	impl_ptr_->healthy = binary_parse::from_datas<bool>(buffer);
	impl_ptr_->group = binary_parse::from_datas<string>(buffer);
	impl_ptr_->weight = binary_parse::from_datas<int32>(buffer);
	impl_ptr_->version = binary_parse::from_datas<string>(buffer);
}
void regist_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("name", impl_ptr_->name);
	jv_obj.emplace("host", impl_ptr_->host);
	jv_obj.emplace("port", impl_ptr_->port);
	jv_obj.emplace("healthy", impl_ptr_->healthy);
	jv_obj.emplace("group", impl_ptr_->group);
	jv_obj.emplace("weight", impl_ptr_->weight);
	jv_obj.emplace("version", impl_ptr_->version);
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void regist_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->name = static_cast<string>(jv_obj->at("name").as_string());
	impl_ptr_->host = static_cast<string>(jv_obj->at("host").as_string());
	impl_ptr_->port = static_cast<int32>(jv_obj->at("port").as_int64());
	impl_ptr_->healthy = jv_obj->at("healthy").as_bool();
	impl_ptr_->group = static_cast<string>(jv_obj->at("group").as_string());
	impl_ptr_->weight = static_cast<int32>(jv_obj->at("weight").as_int64());
	impl_ptr_->version = static_cast<string>(jv_obj->at("version").as_string());
}
void regist_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->name, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->host, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->port, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->healthy, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->group, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->weight, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->version, buffer);
}
void regist_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->name = kv_parse::from_datas<string>(buffer, impl_ptr_->name);
	impl_ptr_->host = kv_parse::from_datas<string>(buffer, impl_ptr_->host);
	impl_ptr_->port = kv_parse::from_datas<int32>(buffer, impl_ptr_->port);
	impl_ptr_->healthy = kv_parse::from_datas<bool>(buffer, impl_ptr_->healthy);
	impl_ptr_->group = kv_parse::from_datas<string>(buffer, impl_ptr_->group);
	impl_ptr_->weight = kv_parse::from_datas<int32>(buffer, impl_ptr_->weight);
	impl_ptr_->version = kv_parse::from_datas<string>(buffer, impl_ptr_->version);
}
string regist_req_body::name() const
{
	return impl_ptr_->name;
}
string& regist_req_body::name()
{
	return impl_ptr_->name;
}
string regist_req_body::host() const
{
	return impl_ptr_->host;
}
string& regist_req_body::host()
{
	return impl_ptr_->host;
}
int32 regist_req_body::port() const
{
	return impl_ptr_->port;
}
int32& regist_req_body::port()
{
	return impl_ptr_->port;
}
bool regist_req_body::healthy() const
{
	return impl_ptr_->healthy;
}
bool& regist_req_body::healthy()
{
	return impl_ptr_->healthy;
}
string regist_req_body::group() const
{
	return impl_ptr_->group;
}
string& regist_req_body::group()
{
	return impl_ptr_->group;
}
int32 regist_req_body::weight() const
{
	return impl_ptr_->weight;
}
int32& regist_req_body::weight()
{
	return impl_ptr_->weight;
}
string regist_req_body::version() const
{
	return impl_ptr_->version;
}
string& regist_req_body::version()
{
	return impl_ptr_->version;
}
struct regist_resp_body::impl
{
};
regist_resp_body::regist_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

regist_resp_body::~regist_resp_body(){}

void regist_resp_body::to_binary(flex_buffer& buffer)
{
}
void regist_resp_body::from_binary(flex_buffer& buffer)
{
}
void regist_resp_body::to_json(flex_buffer& buffer)
{
}
void regist_resp_body::from_json(flex_buffer& buffer)
{
}
void regist_resp_body::to_kv(flex_buffer& buffer)
{
}
void regist_resp_body::from_kv(flex_buffer& buffer)
{
}
