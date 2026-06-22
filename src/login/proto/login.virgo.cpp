#include "login.virgo.h"

struct login_req_body::impl
{
	string username;
	string password;
};
login_req_body::login_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

login_req_body::~login_req_body(){}

void login_req_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<login_req_body>(*this, buffer); 
	}
	else
	{
		buffer.sputc('?');
		kv_base::parse_to(impl_ptr_->username, buffer,"username");
		buffer.sputc('&');
		kv_base::parse_to(impl_ptr_->password, buffer,"password");
	}
}

void login_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<login_req_body>(buffer); 
	}
	else
	{
		impl_ptr_->username = kv_base::parse_from<string>(buffer, "username");
		impl_ptr_->password = kv_base::parse_from<string>(buffer, "password");
	}
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
};
login_resp_body::login_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

login_resp_body::~login_resp_body(){}

void login_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		json_base::parse_.to_datas<login_resp_body>(*this, buffer); 
	}
	else
	{
	}
}

void login_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	if(this->method() == http_method::post)
	{
		*this = json_base::parse_.from_datas<login_resp_body>(buffer); 
	}
	else
	{
	}
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_body& local)
{
	auto& jv_obj = jv.emplace_object();
	jv_obj.emplace("username", local.impl_ptr_->username);
	jv_obj.emplace("password", local.impl_ptr_->password);
}

login_req_body tag_invoke(const aquarius::json::value_to_tag<login_req_body>&, const aquarius::json::value& jv)
{
	login_req_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	result.impl_ptr_->username = static_cast<string>(obj->at("username").as_string());
	result.impl_ptr_->password = static_cast<string>(obj->at("password").as_string());
	return result;
}

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_body& local)
{
	auto& jv_obj = jv.emplace_object();
}

login_resp_body tag_invoke(const aquarius::json::value_to_tag<login_resp_body>&, const aquarius::json::value& jv)
{
	login_resp_body result{};
	auto obj = jv.try_as_object();
	if(obj->empty())
		return {};
	return result;
}
