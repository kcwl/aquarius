#include "regist.virgo.h"

struct regist_req_body::impl
{
	string name;
	string host;
	int32 port;
	bool healthy;
	string group;
	int32 weight;
	string version;
};
regist_req_body::regist_req_body()
	: impl_ptr_(std::make_unique<impl>())
{}

void regist_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void regist_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<regist_req_body::impl>(buffer); 
}
string regist_req_body::name() const
{
	return impl_ptr_->name;
}
string& regist_req_body::name()
{
	return impl_ptr_->name;
}
string regist_req_body::host() const
{
	return impl_ptr_->host;
}
string& regist_req_body::host()
{
	return impl_ptr_->host;
}
int32 regist_req_body::port() const
{
	return impl_ptr_->port;
}
int32& regist_req_body::port()
{
	return impl_ptr_->port;
}
bool regist_req_body::healthy() const
{
	return impl_ptr_->healthy;
}
bool& regist_req_body::healthy()
{
	return impl_ptr_->healthy;
}
string regist_req_body::group() const
{
	return impl_ptr_->group;
}
string& regist_req_body::group()
{
	return impl_ptr_->group;
}
int32 regist_req_body::weight() const
{
	return impl_ptr_->weight;
}
int32& regist_req_body::weight()
{
	return impl_ptr_->weight;
}
string regist_req_body::version() const
{
	return impl_ptr_->version;
}
string& regist_req_body::version()
{
	return impl_ptr_->version;
}
struct regist_resp_body::impl
{
};

void regist_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void regist_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<regist_resp_body::impl>(buffer); 
}
