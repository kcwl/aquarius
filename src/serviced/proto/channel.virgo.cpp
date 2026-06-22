#include "channel.virgo.h"

struct subs_list_req_body::impl
{
};
subs_list_req_body::subs_list_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subs_list_req_body::~subs_list_req_body(){}

void subs_list_req_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<subs_list_req_body>(*this, buffer); 
	}
	else
	{
	}
}

void subs_list_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<subs_list_req_body>(buffer); 
	}
	else
	{
	}
}
struct subs_list_resp_body::impl
{
	std::vector<string> keys;
};
subs_list_resp_body::subs_list_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subs_list_resp_body::~subs_list_resp_body(){}

void subs_list_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<subs_list_resp_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->keys, buffer,"keys");
	}
}

void subs_list_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<subs_list_resp_body>(buffer); 
	}
	else
	{
		impl_ptr_->keys = kv_base::parse_from<std::vector<string>>(buffer, "keys");
	}
}
std::vector<string> subs_list_resp_body::keys() const
{
	return impl_ptr_->keys;
}
std::vector<string>& subs_list_resp_body::keys()
{
	return impl_ptr_->keys;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subs_list_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
}

subs_list_req_body tag_invoke(const aquarius::json::value_to_tag<subs_list_req_body>&, const aquarius::json::value& jv)
{
	subs_list_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subs_list_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("keys", aquarius::json_value_from_object<std::vector<string>>(local.impl_ptr_->keys));
}

subs_list_resp_body tag_invoke(const aquarius::json::value_to_tag<subs_list_resp_body>&, const aquarius::json::value& jv)
{
	subs_list_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->keys = aquarius::json::value_to<std::vector<string>>(obj->at("keys"));
	return result;
}
