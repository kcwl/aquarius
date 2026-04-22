#include "channel.virgo.h"

struct subscribe_req_body::impl
{
	string topic;
};
subscribe_req_body::subscribe_req_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void subscribe_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void subscribe_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<subscribe_req_body>(buffer); 
}
string subscribe_req_body::topic() const
{
	return impl_ptr_->topic;
}
string& subscribe_req_body::topic()
{
	return impl_ptr_->topic;
}
struct subscribe_resp_body::impl
{
};

void subscribe_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void subscribe_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<subscribe_resp_body>(buffer); 
}
