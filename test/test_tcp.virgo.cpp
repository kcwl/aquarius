#include "test_tcp.virgo.h"



struct login_req_body::impl
{
	tcp_person per_req;
};
login_req_body::login_req_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void login_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void login_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<login_req_body::impl>(buffer); 
}
tcp_person login_req_body::per_req() const
{
	return impl_ptr_->per_req;
}
tcp_person& login_req_body::per_req()
{
	return impl_ptr_->per_req;
}
struct login_resp_body::impl
{
	tcp_person per_resp;
};
login_resp_body::login_resp_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void login_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void login_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<login_resp_body::impl>(buffer); 
}
tcp_person login_resp_body::per_resp() const
{
	return impl_ptr_->per_resp;
}
tcp_person& login_resp_body::per_resp()
{
	return impl_ptr_->per_resp;
}
