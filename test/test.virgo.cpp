login::login()
	: per_req()
{}

bool login::operator==(const login& other) const
{
	return per_req == other.per_req;
}

void login::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_req, buffer);
}

void login::deserialize(aquarius::flex_buffer& buffer)
{
	per_req = this->parse_from<person>(buffer);
}
login::login()
	: per_resp()
{}

bool login::operator==(const login& other) const
{
	return per_resp == other.per_resp;
}

void login::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_resp, buffer);
}

void login::deserialize(aquarius::flex_buffer& buffer)
{
	per_resp = this->parse_from<person>(buffer);
}
http_test_post::http_test_post()
	: uuid()
	, per_req()
{}

bool http_test_post::operator==(const http_test_post& other) const
{
	return uuid == other.uuid && per_req == other.per_req;
}

void http_test_post::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_post::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_post>(buffer); 
}
http_test_post::http_test_post()
	: uuid()
	, per_resp()
{}

bool http_test_post::operator==(const http_test_post& other) const
{
	return uuid == other.uuid && per_resp == other.per_resp;
}

void http_test_post::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_post::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_post>(buffer); 
}
http_test_get::http_test_get()
	: user()
	, passwd()
{}

bool http_test_get::operator==(const http_test_get& other) const
{
	return user == other.user && passwd == other.passwd;
}

void http_test_get::serialize(aquarius::flex_buffer& buffer)
{
	buffer.sputc('?');
	this->parse_to(user, buffer, "user");
	buffer.sputc('&');
	this->parse_to(passwd, buffer, "passwd");
}

void http_test_get::deserialize(aquarius::flex_buffer& buffer)
{
	user = this->parse_from<int32>(buffer, "user");
	passwd = this->parse_from<string>(buffer, "passwd");
}
http_test_get::http_test_get()
	: user()
	, passwd()
{}

bool http_test_get::operator==(const http_test_get& other) const
{
	return user == other.user && passwd == other.passwd;
}

void http_test_get::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_get::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_get>(buffer); 
}
