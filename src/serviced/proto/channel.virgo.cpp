#include "channel.virgo.h"
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>

using namespace aquarius;

struct subs_list_req_body::impl
{
};
subs_list_req_body::subs_list_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subs_list_req_body::~subs_list_req_body(){}

void subs_list_req_body::to_binary(flex_buffer& buffer)
{
}
void subs_list_req_body::from_binary(flex_buffer& buffer)
{
}
void subs_list_req_body::to_json(flex_buffer& buffer)
{
}
void subs_list_req_body::from_json(flex_buffer& buffer)
{
}
void subs_list_req_body::to_kv(flex_buffer& buffer)
{
}
void subs_list_req_body::from_kv(flex_buffer& buffer)
{
}
struct subs_list_resp_body::impl
{
	std::vector<string> keys;
};
subs_list_resp_body::subs_list_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subs_list_resp_body::~subs_list_resp_body(){}

void subs_list_resp_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->keys, buffer);
}
void subs_list_resp_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->keys = binary_parse::from_datas<std::vector<string>>(buffer);
}
void subs_list_resp_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("keys", aquarius::json_value_from_object<std::vector<string>>(impl_ptr_->keys));
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void subs_list_resp_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->keys = aquarius::json::value_to<std::vector<string>>(jv_obj->at("keys"));
}
void subs_list_resp_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->keys, buffer);
}
void subs_list_resp_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->keys = kv_parse::from_datas<std::vector<string>>(buffer, impl_ptr_->keys);
}
std::vector<string> subs_list_resp_body::keys() const
{
	return impl_ptr_->keys;
}
std::vector<string>& subs_list_resp_body::keys()
{
	return impl_ptr_->keys;
}
