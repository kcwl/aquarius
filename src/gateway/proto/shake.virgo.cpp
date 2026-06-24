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
	this->parse_to(*this->impl_ptr_, buffer);
}

void shake_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<shake_req_body::impl>(buffer); 
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
	this->parse_to(*this->impl_ptr_, buffer);
}

void shake_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<shake_resp_body::impl>(buffer); 
}
std::vector<string> shake_resp_body::topics() const
{
	return impl_ptr_->topics;
}
std::vector<string>& shake_resp_body::topics()
{
	return impl_ptr_->topics;
}
