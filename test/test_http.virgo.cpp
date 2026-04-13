#include "test_http.virgo.h"


struct http_test_post_req_body::impl
{
	int32 uuid;
	http_person per_req;
};
http_test_post_req_body::http_test_post_req_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void http_test_post_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_post_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_post_req_body>(buffer); 
}
int32 http_test_post_req_body::uuid() const
{
	return impl_ptr_->uuid;
}
int32& http_test_post_req_body::uuid()
{
	return impl_ptr_->uuid;
}
http_person http_test_post_req_body::per_req() const
{
	return impl_ptr_->per_req;
}
http_person& http_test_post_req_body::per_req()
{
	return impl_ptr_->per_req;
}
struct http_test_post_resp_body::impl
{
	int32 uuid;
	http_person per_resp;
};
http_test_post_resp_body::http_test_post_resp_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void http_test_post_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_post_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_post_resp_body>(buffer); 
}
int32 http_test_post_resp_body::uuid() const
{
	return impl_ptr_->uuid;
}
int32& http_test_post_resp_body::uuid()
{
	return impl_ptr_->uuid;
}
http_person http_test_post_resp_body::per_resp() const
{
	return impl_ptr_->per_resp;
}
http_person& http_test_post_resp_body::per_resp()
{
	return impl_ptr_->per_resp;
}

struct http_test_get_req_body::impl
{
	int32 user;
	string passwd;
};
http_test_get_req_body::http_test_get_req_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void http_test_get_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_get_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_get_req_body>(buffer); 
}
int32 http_test_get_req_body::user() const
{
	return impl_ptr_->user;
}
int32& http_test_get_req_body::user()
{
	return impl_ptr_->user;
}
string http_test_get_req_body::passwd() const
{
	return impl_ptr_->passwd;
}
string& http_test_get_req_body::passwd()
{
	return impl_ptr_->passwd;
}
struct http_test_get_resp_body::impl
{
	int32 user;
	string passwd;
};
http_test_get_resp_body::http_test_get_resp_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void http_test_get_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_get_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_get_resp_body>(buffer); 
}
int32 http_test_get_resp_body::user() const
{
	return impl_ptr_->user;
}
int32& http_test_get_resp_body::user()
{
	return impl_ptr_->user;
}
string http_test_get_resp_body::passwd() const
{
	return impl_ptr_->passwd;
}
string& http_test_get_resp_body::passwd()
{
	return impl_ptr_->passwd;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_person& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("sex", local.sex);
	jv_obj.emplace("addr", local.addr);
	jv_obj.emplace("age", local.age);
	jv_obj.emplace("telephone", local.telephone);
	jv_obj.emplace("score", local.score);
	jv_obj.emplace("hp", local.hp);
	jv_obj.emplace("mana", local.mana);
	jv_obj.emplace("name", local.name);
	jv_obj.emplace("orders", aquarius::json_value_from_array(local.orders));
}

http_person tag_invoke(const aquarius::json::value_to_tag<http_person>&, const aquarius::json::value& jv)
{
	http_person result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.sex = obj->at("sex").as_bool();
	result.addr = static_cast<uint32>(obj->at("addr").as_int64());
	result.age = static_cast<int32>(obj->at("age").as_int64());
	result.telephone = static_cast<uint64>(obj->at("telephone").as_int64());
	result.score = obj->at("score").as_int64();
	result.hp = static_cast<float>(obj->at("hp").as_double());
	result.mana = obj->at("mana").as_double();
	result.name = static_cast<string>(obj->at("name").as_string());
	result.orders = aquarius::json_value_to_array(obj->at("orders"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_post_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.impl_ptr_->uuid);
	jv_obj.emplace("per_req", aquarius::json_value_from_object<http_person>(local.impl_ptr_->per_req));
}

http_test_post_req_body tag_invoke(const aquarius::json::value_to_tag<http_test_post_req_body>&, const aquarius::json::value& jv)
{
	http_test_post_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.impl_ptr_->per_req = aquarius::json::value_to<http_person>(obj->at("per_req"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_post_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("uuid", local.impl_ptr_->uuid);
	jv_obj.emplace("per_resp", aquarius::json_value_from_object<http_person>(local.impl_ptr_->per_resp));
}

http_test_post_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_post_resp_body>&, const aquarius::json::value& jv)
{
	http_test_post_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->uuid = static_cast<int32>(obj->at("uuid").as_int64());
	result.impl_ptr_->per_resp = aquarius::json::value_to<http_person>(obj->at("per_resp"));
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user", local.impl_ptr_->user);
	jv_obj.emplace("passwd", local.impl_ptr_->passwd);
}

http_test_get_req_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_req_body>&, const aquarius::json::value& jv)
{
	http_test_get_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->user = static_cast<int32>(obj->at("user").as_int64());
	result.impl_ptr_->passwd = static_cast<string>(obj->at("passwd").as_string());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("user", local.impl_ptr_->user);
	jv_obj.emplace("passwd", local.impl_ptr_->passwd);
}

http_test_get_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_resp_body>&, const aquarius::json::value& jv)
{
	http_test_get_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->user = static_cast<int32>(obj->at("user").as_int64());
	result.impl_ptr_->passwd = static_cast<string>(obj->at("passwd").as_string());
	return result;
}
