#pragma once
#include <aquarius.hpp>

class login_req_body : public aquarius::tcp_binary_serialize
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

class login_resp_body : public aquarius::tcp_binary_serialize
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

std::ostream& operator<<(std::ostream& os, const login_req_body& other);
std::ostream& operator<<(std::ostream& os, const login_resp_body& other);

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_body& local);
login_req_body tag_invoke(const aquarius::json::value_to_tag<login_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_body& local);
login_resp_body tag_invoke(const aquarius::json::value_to_tag<login_resp_body>&, const aquarius::json::value& jv);

using login_tcp_request = aquarius::virgo::tcp_request<"10000", aquarius::tcp_request_header, login_req_body>;
using login_tcp_response = aquarius::virgo::tcp_response<aquarius::tcp_response_header, login_resp_body>;
