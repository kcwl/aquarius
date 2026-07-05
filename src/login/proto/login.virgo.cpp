#include "login.virgo.h"
#include <aquarius/serialize/binary.hpp>
#include <aquarius/serialize/json.hpp>
#include <aquarius/serialize/kv.hpp>

using namespace aquarius;

struct login_req_body::impl
{
	string username;
	string password;
AQUARIUS_SERIALIZE_STRUCT(login_req_body::impl, (username, password))
};
login_req_body::login_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

login_req_body::~login_req_body(){}

void login_req_body::to_binary(flex_buffer& buffer)
{
	binary_parse::to_datas(impl_ptr_->username, buffer);
	binary_parse::to_datas(impl_ptr_->password, buffer);
}
void login_req_body::from_binary(flex_buffer& buffer)
{
	impl_ptr_->username = binary_parse::from_datas<string>(buffer);
	impl_ptr_->password = binary_parse::from_datas<string>(buffer);
}
void login_req_body::to_json(flex_buffer& buffer)
{
	json::value jv{};
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("username", impl_ptr_->username);
	jv_obj.emplace("password", impl_ptr_->password);
	auto json_str = json::serialize(jv);
	buffer.sputn(json_str.data(), json_str.size());
}
void login_req_body::from_json(flex_buffer& buffer)
{
	json::value jv = json::parse(std::string((char*)buffer.data().data(), buffer.size()));
	auto jv_obj = jv.try_as_object();
	if(jv_obj.has_error())
	{
		return;
	}
	impl_ptr_->username = static_cast<string>(jv_obj->at("username").as_string());
	impl_ptr_->password = static_cast<string>(jv_obj->at("password").as_string());
}
void login_req_body::to_kv(flex_buffer& buffer)
{
	buffer.sputc('?');
	kv_parse::to_datas(impl_ptr_->username, buffer);
buffer.sputc('=');
	kv_parse::to_datas(impl_ptr_->password, buffer);
}
void login_req_body::from_kv(flex_buffer& buffer)
{
	impl_ptr_->username = kv_parse::from_datas<string>(buffer, impl_ptr_->username);
	impl_ptr_->password = kv_parse::from_datas<string>(buffer, impl_ptr_->password);
}
string login_req_body::username() const
{
	return impl_ptr_->username;
}
string& login_req_body::username()
{
	return impl_ptr_->username;
}
string login_req_body::password() const
{
	return impl_ptr_->password;
}
string& login_req_body::password()
{
	return impl_ptr_->password;
}
struct login_resp_body::impl
{
AQUARIUS_SERIALIZE_STRUCT(login_resp_body::impl, ())
};
login_resp_body::login_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

login_resp_body::~login_resp_body(){}

void login_resp_body::to_binary(flex_buffer& buffer)
{
}
void login_resp_body::from_binary(flex_buffer& buffer)
{
}
void login_resp_body::to_json(flex_buffer& buffer)
{
}
void login_resp_body::from_json(flex_buffer& buffer)
{
}
void login_resp_body::to_kv(flex_buffer& buffer)
{
}
void login_resp_body::from_kv(flex_buffer& buffer)
{
}
