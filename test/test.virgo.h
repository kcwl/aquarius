#pragma once
#include <aquarius.hpp>

struct person
{
	bool operator==(const person & other) const; 

	bool sex;
	uint32 addr;
	int32 age;
	uint64 telephone;
	int64 score;
	float hp;
	double mana;
	string name;
	bytes orders;
};

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
	person per_req;
};

class login_resp_body : public aquarius::tcp_binary_serialize
{
public:
	login_resp_body();
	virtual ~login_resp_body() = default;

public:
	bool operator==(const login_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
};

class new_http_login_req_body : public aquarius::http_json_serialize
{
public:
	new_http_login_req_body();
	virtual ~new_http_login_req_body() = default;

public:
	bool operator==(const new_http_login_req_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
	person per_req;
};

class new_http_login_resp_body : public aquarius::http_json_serialize
{
public:
	new_http_login_resp_body();
	virtual ~new_http_login_resp_body() = default;

public:
	bool operator==(const new_http_login_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
	person per_resp;
};

class http_test_get_req_body : public aquarius::http_kv_serialize
{
public:
	http_test_get_req_body();
	virtual ~http_test_get_req_body() = default;

public:
	bool operator==(const http_test_get_req_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 user;
	string passwd;
};

class http_test_get_resp_body : public aquarius::http_json_serialize
{
public:
	http_test_get_resp_body();
	virtual ~http_test_get_resp_body() = default;

public:
	bool operator==(const http_test_get_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 user;
	string passwd;
};

std::ostream& operator<<(std::ostream& os, const person& other);
std::ostream& operator<<(std::ostream& os, const login_req_body& other);
std::ostream& operator<<(std::ostream& os, const login_resp_body& other);
std::ostream& operator<<(std::ostream& os, const new_http_login_req_body& other);
std::ostream& operator<<(std::ostream& os, const new_http_login_resp_body& other);
std::ostream& operator<<(std::ostream& os, const http_test_get_req_body& other);
std::ostream& operator<<(std::ostream& os, const http_test_get_resp_body& other);

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const person& local);
person tag_invoke(const aquarius::json::value_to_tag<person>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_body& local);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const person& local);
login_req_body tag_invoke(const aquarius::json::value_to_tag<login_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_body& local);
login_resp_body tag_invoke(const aquarius::json::value_to_tag<login_resp_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const new_http_login_req_body& local);
new_http_login_req_body tag_invoke(const aquarius::json::value_to_tag<new_http_login_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const new_http_login_resp_body& local);
new_http_login_resp_body tag_invoke(const aquarius::json::value_to_tag<new_http_login_resp_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_req_body& local);
http_test_get_req_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_resp_body& local);
http_test_get_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_resp_body>&, const aquarius::json::value& jv);

using login_tcp_request = aquarius::virgo::tcp_request<"10001", aquarius::tcp_request_header, login_req_body>;
using login_tcp_response = aquarius::virgo::tcp_response<aquarius::tcp_response_header, login_resp_body>;
using new_http_login_http_request = aquarius::virgo::http_request<"/login/new", aquarius::virgo::http_method::post, aquarius::http_request_header, new_http_login_req_body>;
using new_http_login_http_response = aquarius::virgo::http_response<aquarius::virgo::http_method::post, aquarius::http_response_header, new_http_login_resp_body>;
using http_test_get_http_request = aquarius::virgo::http_request<"/login", aquarius::virgo::http_method::get, aquarius::http_request_header, http_test_get_req_body>;
using http_test_get_http_response = aquarius::virgo::http_response<aquarius::virgo::http_method::get, aquarius::http_response_header, http_test_get_resp_body>;
