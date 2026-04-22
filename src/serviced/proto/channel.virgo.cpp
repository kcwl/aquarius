#include "channel.virgo.h"

struct subs_list_req_body::impl
{
};

void subs_list_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void subs_list_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<subs_list_req_body>(buffer); 
}
struct subs_list_resp_body::impl
{
	std::vector<string> keys;
};
subs_list_resp_body::subs_list_resp_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void subs_list_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void subs_list_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<subs_list_resp_body>(buffer); 
}
std::vector<string> subs_list_resp_body::keys() const
{
	return impl_ptr_->keys;
}
std::vector<string>& subs_list_resp_body::keys()
{
	return impl_ptr_->keys;
}
