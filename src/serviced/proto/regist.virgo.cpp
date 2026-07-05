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

struct subscribe_service_req_body::impl
{
	string group;
};
subscribe_service_req_body::subscribe_service_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subscribe_service_req_body::~subscribe_service_req_body(){}

void subscribe_service_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->group, buffer);
}
void subscribe_service_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->group = binary_parse::from_datas<string>(buffer);
}
void subscribe_service_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("group", impl_ptr_->group);
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void subscribe_service_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->group = static_cast<string>(jv_obj->at("group").as_string());
}
void subscribe_service_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->group, buffer);
}
void subscribe_service_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->group = kv_parse::from_datas<string>(buffer, impl_ptr_->group);
}
string subscribe_service_req_body::group() const
{
	return impl_ptr_->group;
}
string& subscribe_service_req_body::group()
{
	return impl_ptr_->group;
}
struct subscribe_service_resp_body::impl
{
	std::vector<uint64_t> instances;
};
subscribe_service_resp_body::subscribe_service_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subscribe_service_resp_body::~subscribe_service_resp_body(){}

void subscribe_service_resp_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->instances, buffer);
}
void subscribe_service_resp_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->instances = binary_parse::from_datas<std::vector<uint64_t>>(buffer);
}
void subscribe_service_resp_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("instances", aquarius::json_value_from_object<std::vector<uint64_t>>(impl_ptr_->instances));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void subscribe_service_resp_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->instances = aquarius::json::value_to<std::vector<uint64_t>>(jv_obj->at("instances"));
}
void subscribe_service_resp_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->instances, buffer);
}
void subscribe_service_resp_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->instances = kv_parse::from_datas<std::vector<uint64_t>>(buffer, impl_ptr_->instances);
}
std::vector<uint64_t> subscribe_service_resp_body::instances() const
{
	return impl_ptr_->instances;
}
std::vector<uint64_t>& subscribe_service_resp_body::instances()
{
	return impl_ptr_->instances;
}

struct broad_service_status_req_body::impl
{
};
broad_service_status_req_body::broad_service_status_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

broad_service_status_req_body::~broad_service_status_req_body(){}

void broad_service_status_req_body::to_binary(flex_buffer& buffer)
{
}
void broad_service_status_req_body::from_binary(flex_buffer& buffer)
{
}
void broad_service_status_req_body::to_json(flex_buffer& buffer)
{
}
void broad_service_status_req_body::from_json(flex_buffer& buffer)
{
}
void broad_service_status_req_body::to_kv(flex_buffer& buffer)
{
}
void broad_service_status_req_body::from_kv(flex_buffer& buffer)
{
}
struct broad_service_status_resp_body::impl
{
	string group;
	uint64 host_and_port;
	bool healty;
};
broad_service_status_resp_body::broad_service_status_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

broad_service_status_resp_body::~broad_service_status_resp_body(){}

void broad_service_status_resp_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->group, buffer);
	binary_parse::to_datas(impl_ptr_->host_and_port, buffer);
	binary_parse::to_datas(impl_ptr_->healty, buffer);
}
void broad_service_status_resp_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->group = binary_parse::from_datas<string>(buffer);
	impl_ptr_->host_and_port = binary_parse::from_datas<uint64>(buffer);
	impl_ptr_->healty = binary_parse::from_datas<bool>(buffer);
}
void broad_service_status_resp_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("group", impl_ptr_->group);
	jv_obj.emplace("host_and_port", impl_ptr_->host_and_port);
	jv_obj.emplace("healty", impl_ptr_->healty);
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void broad_service_status_resp_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->group = static_cast<string>(jv_obj->at("group").as_string());
	impl_ptr_->host_and_port = static_cast<uint64>(jv_obj->at("host_and_port").as_int64());
	impl_ptr_->healty = jv_obj->at("healty").as_bool();
}
void broad_service_status_resp_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->group, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->host_and_port, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->healty, buffer);
}
void broad_service_status_resp_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->group = kv_parse::from_datas<string>(buffer, impl_ptr_->group);
	impl_ptr_->host_and_port = kv_parse::from_datas<uint64>(buffer, impl_ptr_->host_and_port);
	impl_ptr_->healty = kv_parse::from_datas<bool>(buffer, impl_ptr_->healty);
}
string broad_service_status_resp_body::group() const
{
	return impl_ptr_->group;
}
string& broad_service_status_resp_body::group()
{
	return impl_ptr_->group;
}
uint64 broad_service_status_resp_body::host_and_port() const
{
	return impl_ptr_->host_and_port;
}
uint64& broad_service_status_resp_body::host_and_port()
{
	return impl_ptr_->host_and_port;
}
bool broad_service_status_resp_body::healty() const
{
	return impl_ptr_->healty;
}
bool& broad_service_status_resp_body::healty()
{
	return impl_ptr_->healty;
}
