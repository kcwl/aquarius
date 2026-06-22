#include "shake.virgo.h"

struct shake_req_body::impl
{
};
shake_req_body::shake_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

shake_req_body::~shake_req_body(){}

void shake_req_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<shake_req_body>(*this, buffer); 
	}
	else
	{
	}
}

void shake_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<shake_req_body>(buffer); 
	}
	else
	{
	}
}
struct shake_resp_body::impl
{
	std::vector<string> topics;
};
shake_resp_body::shake_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

shake_resp_body::~shake_resp_body(){}

void shake_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<shake_resp_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->topics, buffer,"topics");
	}
}

void shake_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<shake_resp_body>(buffer); 
	}
	else
	{
		impl_ptr_->topics = kv_base::parse_from<std::vector<string>>(buffer, "topics");
	}
}
std::vector<string> shake_resp_body::topics() const
{
	return impl_ptr_->topics;
}
std::vector<string>& shake_resp_body::topics()
{
	return impl_ptr_->topics;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const shake_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
}

shake_req_body tag_invoke(const aquarius::json::value_to_tag<shake_req_body>&, const aquarius::json::value& jv)
{
	shake_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const shake_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("topics", aquarius::json_value_from_object<std::vector<string>>(local.impl_ptr_->topics));
}

shake_resp_body tag_invoke(const aquarius::json::value_to_tag<shake_resp_body>&, const aquarius::json::value& jv)
{
	shake_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->topics = aquarius::json::value_to<std::vector<string>>(obj->at("topics"));
	return result;
}
