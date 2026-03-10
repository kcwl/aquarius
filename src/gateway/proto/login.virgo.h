#pragma once
#include <aquarius.hpp>

class login_req_body : public aquarius::http_json_serialize
{
public:
	login_req_body();
	virtual ~login_req_body() = default;

public:
	bool operator==(const login_req_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	string user;
	string passwd;
};

class login_resp_body : public aquarius::http_json_serialize
{
public:
	login_resp_body() = default;
	virtual ~login_resp_body() = default;

public:
	bool operator==(const login_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
};


void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_body& local);
login_req_body tag_invoke(const aquarius::json::value_to_tag<login_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_body& local);
login_resp_body tag_invoke(const aquarius::json::value_to_tag<login_resp_body>&, const aquarius::json::value& jv);

using login_request = aquarius::virgo::http_request<"/login", aquarius::virgo::http_method::post, login_req_body>;
using login_response = aquarius::virgo::http_response<aquarius::virgo::http_method::post, login_resp_body>;
