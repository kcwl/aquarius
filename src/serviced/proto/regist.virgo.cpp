#include "regist.virgo.h"

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

void regist_req_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<regist_req_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->name, buffer,"name");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->host, buffer,"host");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->port, buffer,"port");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->healthy, buffer,"healthy");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->group, buffer,"group");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->weight, buffer,"weight");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->version, buffer,"version");
	}
}

void regist_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<regist_req_body>(buffer); 
	}
	else
	{
		impl_ptr_->name = kv_base::parse_from<string>(buffer, "name");
		impl_ptr_->host = kv_base::parse_from<string>(buffer, "host");
		impl_ptr_->port = kv_base::parse_from<int32>(buffer, "port");
		impl_ptr_->healthy = kv_base::parse_from<bool>(buffer, "healthy");
		impl_ptr_->group = kv_base::parse_from<string>(buffer, "group");
		impl_ptr_->weight = kv_base::parse_from<int32>(buffer, "weight");
		impl_ptr_->version = kv_base::parse_from<string>(buffer, "version");
	}
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

void regist_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<regist_resp_body>(*this, buffer); 
	}
	else
	{
	}
}

void regist_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<regist_resp_body>(buffer); 
	}
	else
	{
	}
}

struct subscribe_service_req_body::impl
{
	string group;
};
subscribe_service_req_body::subscribe_service_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subscribe_service_req_body::~subscribe_service_req_body(){}

void subscribe_service_req_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<subscribe_service_req_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->group, buffer,"group");
	}
}

void subscribe_service_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<subscribe_service_req_body>(buffer); 
	}
	else
	{
		impl_ptr_->group = kv_base::parse_from<string>(buffer, "group");
	}
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

void subscribe_service_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<subscribe_service_resp_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->instances, buffer,"instances");
	}
}

void subscribe_service_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<subscribe_service_resp_body>(buffer); 
	}
	else
	{
		impl_ptr_->instances = kv_base::parse_from<std::vector<uint64_t>>(buffer, "instances");
	}
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

void broad_service_status_req_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<broad_service_status_req_body>(*this, buffer); 
	}
	else
	{
	}
}

void broad_service_status_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<broad_service_status_req_body>(buffer); 
	}
	else
	{
	}
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

void broad_service_status_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<broad_service_status_resp_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->group, buffer,"group");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->host_and_port, buffer,"host_and_port");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->healty, buffer,"healty");
	}
}

void broad_service_status_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<broad_service_status_resp_body>(buffer); 
	}
	else
	{
		impl_ptr_->group = kv_base::parse_from<string>(buffer, "group");
		impl_ptr_->host_and_port = kv_base::parse_from<uint64>(buffer, "host_and_port");
		impl_ptr_->healty = kv_base::parse_from<bool>(buffer, "healty");
	}
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

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const regist_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("name", local.impl_ptr_->name);
	jv_obj.emplace("host", local.impl_ptr_->host);
	jv_obj.emplace("port", local.impl_ptr_->port);
	jv_obj.emplace("healthy", local.impl_ptr_->healthy);
	jv_obj.emplace("group", local.impl_ptr_->group);
	jv_obj.emplace("weight", local.impl_ptr_->weight);
	jv_obj.emplace("version", local.impl_ptr_->version);
}

regist_req_body tag_invoke(const aquarius::json::value_to_tag<regist_req_body>&, const aquarius::json::value& jv)
{
	regist_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->name = static_cast<string>(obj->at("name").as_string());
	result.impl_ptr_->host = static_cast<string>(obj->at("host").as_string());
	result.impl_ptr_->port = static_cast<int32>(obj->at("port").as_int64());
	result.impl_ptr_->healthy = obj->at("healthy").as_bool();
	result.impl_ptr_->group = static_cast<string>(obj->at("group").as_string());
	result.impl_ptr_->weight = static_cast<int32>(obj->at("weight").as_int64());
	result.impl_ptr_->version = static_cast<string>(obj->at("version").as_string());
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

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subscribe_service_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("group", local.impl_ptr_->group);
}

subscribe_service_req_body tag_invoke(const aquarius::json::value_to_tag<subscribe_service_req_body>&, const aquarius::json::value& jv)
{
	subscribe_service_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->group = static_cast<string>(obj->at("group").as_string());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subscribe_service_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("instances", aquarius::json_value_from_object<std::vector<uint64_t>>(local.impl_ptr_->instances));
}

subscribe_service_resp_body tag_invoke(const aquarius::json::value_to_tag<subscribe_service_resp_body>&, const aquarius::json::value& jv)
{
	subscribe_service_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->instances = aquarius::json::value_to<std::vector<uint64_t>>(obj->at("instances"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const broad_service_status_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
}

broad_service_status_req_body tag_invoke(const aquarius::json::value_to_tag<broad_service_status_req_body>&, const aquarius::json::value& jv)
{
	broad_service_status_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const broad_service_status_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("group", local.impl_ptr_->group);
	jv_obj.emplace("host_and_port", local.impl_ptr_->host_and_port);
	jv_obj.emplace("healty", local.impl_ptr_->healty);
}

broad_service_status_resp_body tag_invoke(const aquarius::json::value_to_tag<broad_service_status_resp_body>&, const aquarius::json::value& jv)
{
	broad_service_status_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->group = static_cast<string>(obj->at("group").as_string());
	result.impl_ptr_->host_and_port = static_cast<uint64>(obj->at("host_and_port").as_int64());
	result.impl_ptr_->healty = obj->at("healty").as_bool();
	return result;
}
