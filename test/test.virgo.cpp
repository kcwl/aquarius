login_req_body::login_req_body()
	: per_req()
{}

bool login_req_body::operator==(const login_req_body& other) const
{
	return per_req == other.per_req;
}

void login_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_req, buffer);
}

void login_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	per_req = this->parse_from<person>(buffer);
}
login_resp_body::login_resp_body()
	: per_resp()
{}

bool login_resp_body::operator==(const login_resp_body& other) const
{
	return per_resp == other.per_resp;
}

void login_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(per_resp, buffer);
}

void login_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	per_resp = this->parse_from<person>(buffer);
}
http_test_post_req_body::http_test_post_req_body()
	: uuid()
	, per_req()
{}

bool http_test_post_req_body::operator==(const http_test_post_req_body& other) const
{
	return uuid == other.uuid && per_req == other.per_req;
}

void http_test_post_req_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_post_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_post_req_body>(buffer); 
}
http_test_post_resp_body::http_test_post_resp_body()
	: uuid()
	, per_resp()
{}

bool http_test_post_resp_body::operator==(const http_test_post_resp_body& other) const
{
	return uuid == other.uuid && per_resp == other.per_resp;
}

void http_test_post_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_post_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_post_resp_body>(buffer); 
}
http_test_get_req_body::http_test_get_req_body()
	: user()
	, passwd()
{}

bool http_test_get_req_body::operator==(const http_test_get_req_body& other) const
{
	return user == other.user && passwd == other.passwd;
}

void http_test_get_req_body::serialize(aquarius::flex_buffer& buffer)
{
	buffer.sputc('?');
	this->parse_to(user, buffer, "user");
	buffer.sputc('&');
	this->parse_to(passwd, buffer, "passwd");
}

void http_test_get_req_body::deserialize(aquarius::flex_buffer& buffer)
{
	user = this->parse_from<int32>(buffer, "user");
	passwd = this->parse_from<string>(buffer, "passwd");
}
http_test_get_resp_body::http_test_get_resp_body()
	: user()
	, passwd()
{}

bool http_test_get_resp_body::operator==(const http_test_get_resp_body& other) const
{
	return user == other.user && passwd == other.passwd;
}

void http_test_get_resp_body::serialize(aquarius::flex_buffer& buffer)
{
	this->parse_to(*this, buffer);
}

void http_test_get_resp_body::deserialize(aquarius::flex_buffer& buffer)
{
	*this = this->parse_from<http_test_get_resp_body>(buffer); 
}
