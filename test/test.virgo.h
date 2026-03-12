#pragma once
#include <aquarius.hpp>
#pragma once;
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

class http_test_post_req_body : public aquarius::http_json_serialize
{
public:
	http_test_post_req_body();
	virtual ~http_test_post_req_body() = default;

public:
	bool operator==(const http_test_post_req_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
	person per_req;
};
class http_test_post_resp_body : public aquarius::http_json_serialize
{
public:
	http_test_post_resp_body();
	virtual ~http_test_post_resp_body() = default;

public:
	bool operator==(const http_test_post_resp_body & other) const; 

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

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const person& local);
person tag_invoke(const aquarius::json::value_to_tag<person>&, const aquarius::json::value& jv);
using login_req_body_request = aquarius::virgo::tcp_request<"10001", login_req_body>;
using login_resp_body_response = aquarius::virgo::tcp_response<login_resp_body>;
using http_test_post_req_body_request = aquarius::virgo::tcp_request<"/login/new", http_test_post_req_body>;
using http_test_post_resp_body_response = aquarius::virgo::tcp_response<http_test_post_resp_body>;
using http_test_get_req_body_request = aquarius::virgo::tcp_request<"/login", http_test_get_req_body>;
using http_test_get_resp_body_response = aquarius::virgo::tcp_response<http_test_get_resp_body>;
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const person& local);
person tag_invoke(const aquarius::json::value_to_tag<person>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login& local);
login tag_invoke(const aquarius::json::value_to_tag<login>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_post& local);
http_test_post tag_invoke(const aquarius::json::value_to_tag<http_test_post>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get& local);
http_test_get tag_invoke(const aquarius::json::value_to_tag<http_test_get>&, const aquarius::json::value& jv);
