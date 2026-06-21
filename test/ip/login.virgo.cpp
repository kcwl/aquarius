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
	this->parse_to(*this->impl_ptr_, buffer);
}

void login_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<login_req_body::impl>(buffer); 
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
	this->parse_to(*this->impl_ptr_, buffer);
}

void login_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<login_resp_body::impl>(buffer); 
}
