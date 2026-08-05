#include "shake.virgo.h"
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>

using namespace aquarius;

struct shake_req_body::impl
{
};
shake_req_body::shake_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

shake_req_body::~shake_req_body(){}

void shake_req_body::to_binary(flex_buffer& buffer)
{
}
void shake_req_body::from_binary(flex_buffer& buffer)
{
}
void shake_req_body::to_json(flex_buffer& buffer)
{
}
void shake_req_body::from_json(flex_buffer& buffer)
{
}
void shake_req_body::to_kv(flex_buffer& buffer)
{
}
void shake_req_body::from_kv(flex_buffer& buffer)
{
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
