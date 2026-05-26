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
	: impl_ptr_(std::make_shared<impl>())
{}

regist_req_body::~regist_req_body(){}

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
regist_resp_body::regist_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

regist_resp_body::~regist_resp_body(){}

void regist_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void regist_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<regist_resp_body::impl>(buffer); 
}


struct subscribe_service_req_body::impl
{
	string group;
};
subscribe_service_req_body::subscribe_service_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subscribe_service_req_body::~subscribe_service_req_body(){}

void subscribe_service_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void subscribe_service_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<subscribe_service_req_body::impl>(buffer); 
}
string subscribe_service_req_body::group() const
{
	return impl_ptr_->group;
}
string& subscribe_service_req_body::group()
{
	return impl_ptr_->group;
}
struct subscribe_service_resp_body::impl
{
	std::vector<instance> instances;
};
subscribe_service_resp_body::subscribe_service_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

subscribe_service_resp_body::~subscribe_service_resp_body(){}

void subscribe_service_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void subscribe_service_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<subscribe_service_resp_body::impl>(buffer); 
}
std::vector<instance> subscribe_service_resp_body::instances() const
{
	return impl_ptr_->instances;
}
std::vector<instance>& subscribe_service_resp_body::instances()
{
	return impl_ptr_->instances;
}

struct broad_service_status_req_body::impl
{
};
broad_service_status_req_body::broad_service_status_req_body()
	: impl_ptr_(std::make_shared<impl>())
{}

broad_service_status_req_body::~broad_service_status_req_body(){}

void broad_service_status_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void broad_service_status_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<broad_service_status_req_body::impl>(buffer); 
}
struct broad_service_status_resp_body::impl
{
	string group;
	string host;
	int32 port;
	bool healty;
};
broad_service_status_resp_body::broad_service_status_resp_body()
	: impl_ptr_(std::make_shared<impl>())
{}

broad_service_status_resp_body::~broad_service_status_resp_body(){}

void broad_service_status_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this->impl_ptr_, buffer);
}

void broad_service_status_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this->impl_ptr_ = this->parse_from<broad_service_status_resp_body::impl>(buffer); 
}
string broad_service_status_resp_body::group() const
{
	return impl_ptr_->group;
}
string& broad_service_status_resp_body::group()
{
	return impl_ptr_->group;
}
string broad_service_status_resp_body::host() const
{
	return impl_ptr_->host;
}
string& broad_service_status_resp_body::host()
{
	return impl_ptr_->host;
}
int32 broad_service_status_resp_body::port() const
{
	return impl_ptr_->port;
}
int32& broad_service_status_resp_body::port()
{
	return impl_ptr_->port;
}
bool broad_service_status_resp_body::healty() const
{
	return impl_ptr_->healty;
}
bool& broad_service_status_resp_body::healty()
{
	return impl_ptr_->healty;
}
