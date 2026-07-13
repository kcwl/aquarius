#include "permission.virgo.h"
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>

using namespace aquarius;

struct query_permission_req_body::impl
{
	sint64 user_id;
AQUARIUS_SERIALIZE_STRUCT(query_permission_req_body::impl, (user_id))
};
query_permission_req_body::query_permission_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

query_permission_req_body::~query_permission_req_body(){}

void query_permission_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->user_id, buffer);
}
void query_permission_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->user_id = binary_parse::from_datas<sint64>(buffer);
}
void query_permission_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user_id", aquarius::json_value_from_object<sint64>(impl_ptr_->user_id));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void query_permission_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->user_id = aquarius::json::value_to<sint64>(jv_obj->at("user_id"));
}
void query_permission_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->user_id, buffer);
}
void query_permission_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->user_id = kv_parse::from_datas<sint64>(buffer, impl_ptr_->user_id);
}
sint64 query_permission_req_body::user_id() const
{
	return impl_ptr_->user_id;
}
sint64& query_permission_req_body::user_id()
{
	return impl_ptr_->user_id;
}
struct query_permission_resp_body::impl
{
	std::vector<string> permissions;
AQUARIUS_SERIALIZE_STRUCT(query_permission_resp_body::impl, (permissions))
};
query_permission_resp_body::query_permission_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

query_permission_resp_body::~query_permission_resp_body(){}

void query_permission_resp_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->permissions, buffer);
}
void query_permission_resp_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->permissions = binary_parse::from_datas<std::vector<string>>(buffer);
}
void query_permission_resp_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("permissions", aquarius::json_value_from_object<std::vector<string>>(impl_ptr_->permissions));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void query_permission_resp_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->permissions = aquarius::json::value_to<std::vector<string>>(jv_obj->at("permissions"));
}
void query_permission_resp_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->permissions, buffer);
}
void query_permission_resp_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->permissions = kv_parse::from_datas<std::vector<string>>(buffer, impl_ptr_->permissions);
}
std::vector<string> query_permission_resp_body::permissions() const
{
	return impl_ptr_->permissions;
}
std::vector<string>& query_permission_resp_body::permissions()
{
	return impl_ptr_->permissions;
}

struct check_permission_req_body::impl
{
	sint64 user_id;
	sint64 permission;
AQUARIUS_SERIALIZE_STRUCT(check_permission_req_body::impl, (user_id, permission))
};
check_permission_req_body::check_permission_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

check_permission_req_body::~check_permission_req_body(){}

void check_permission_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->user_id, buffer);
	binary_parse::to_datas(impl_ptr_->permission, buffer);
}
void check_permission_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->user_id = binary_parse::from_datas<sint64>(buffer);
	impl_ptr_->permission = binary_parse::from_datas<sint64>(buffer);
}
void check_permission_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user_id", aquarius::json_value_from_object<sint64>(impl_ptr_->user_id));
	jv_obj.emplace("permission", aquarius::json_value_from_object<sint64>(impl_ptr_->permission));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void check_permission_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->user_id = aquarius::json::value_to<sint64>(jv_obj->at("user_id"));
	impl_ptr_->permission = aquarius::json::value_to<sint64>(jv_obj->at("permission"));
}
void check_permission_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->user_id, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->permission, buffer);
}
void check_permission_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->user_id = kv_parse::from_datas<sint64>(buffer, impl_ptr_->user_id);
	impl_ptr_->permission = kv_parse::from_datas<sint64>(buffer, impl_ptr_->permission);
}
sint64 check_permission_req_body::user_id() const
{
	return impl_ptr_->user_id;
}
sint64& check_permission_req_body::user_id()
{
	return impl_ptr_->user_id;
}
sint64 check_permission_req_body::permission() const
{
	return impl_ptr_->permission;
}
sint64& check_permission_req_body::permission()
{
	return impl_ptr_->permission;
}
struct check_permission_resp_body::impl
{
AQUARIUS_SERIALIZE_STRUCT(check_permission_resp_body::impl, ())
};
check_permission_resp_body::check_permission_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

check_permission_resp_body::~check_permission_resp_body(){}

void check_permission_resp_body::to_binary(flex_buffer& buffer)
{
}
void check_permission_resp_body::from_binary(flex_buffer& buffer)
{
}
void check_permission_resp_body::to_json(flex_buffer& buffer)
{
}
void check_permission_resp_body::from_json(flex_buffer& buffer)
{
}
void check_permission_resp_body::to_kv(flex_buffer& buffer)
{
}
void check_permission_resp_body::from_kv(flex_buffer& buffer)
{
}

struct add_permission_req_body::impl
{
	sint64 user_id;
	std::vector<sint64> permissions;
AQUARIUS_SERIALIZE_STRUCT(add_permission_req_body::impl, (user_id, permissions))
};
add_permission_req_body::add_permission_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

add_permission_req_body::~add_permission_req_body(){}

void add_permission_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->user_id, buffer);
	binary_parse::to_datas(impl_ptr_->permissions, buffer);
}
void add_permission_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->user_id = binary_parse::from_datas<sint64>(buffer);
	impl_ptr_->permissions = binary_parse::from_datas<std::vector<sint64>>(buffer);
}
void add_permission_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user_id", aquarius::json_value_from_object<sint64>(impl_ptr_->user_id));
	jv_obj.emplace("permissions", aquarius::json_value_from_object<std::vector<sint64>>(impl_ptr_->permissions));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void add_permission_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->user_id = aquarius::json::value_to<sint64>(jv_obj->at("user_id"));
	impl_ptr_->permissions = aquarius::json::value_to<std::vector<sint64>>(jv_obj->at("permissions"));
}
void add_permission_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->user_id, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->permissions, buffer);
}
void add_permission_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->user_id = kv_parse::from_datas<sint64>(buffer, impl_ptr_->user_id);
	impl_ptr_->permissions = kv_parse::from_datas<std::vector<sint64>>(buffer, impl_ptr_->permissions);
}
sint64 add_permission_req_body::user_id() const
{
	return impl_ptr_->user_id;
}
sint64& add_permission_req_body::user_id()
{
	return impl_ptr_->user_id;
}
std::vector<sint64> add_permission_req_body::permissions() const
{
	return impl_ptr_->permissions;
}
std::vector<sint64>& add_permission_req_body::permissions()
{
	return impl_ptr_->permissions;
}
struct add_permission_resp_body::impl
{
AQUARIUS_SERIALIZE_STRUCT(add_permission_resp_body::impl, ())
};
add_permission_resp_body::add_permission_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

add_permission_resp_body::~add_permission_resp_body(){}

void add_permission_resp_body::to_binary(flex_buffer& buffer)
{
}
void add_permission_resp_body::from_binary(flex_buffer& buffer)
{
}
void add_permission_resp_body::to_json(flex_buffer& buffer)
{
}
void add_permission_resp_body::from_json(flex_buffer& buffer)
{
}
void add_permission_resp_body::to_kv(flex_buffer& buffer)
{
}
void add_permission_resp_body::from_kv(flex_buffer& buffer)
{
}
