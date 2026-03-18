#pragma once
#include <aquarius.hpp>
using namespace aquarius;

struct http_person
{
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
	http_person per_req;
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
	http_person per_resp;
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

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_person& local);
http_person tag_invoke(const aquarius::json::value_to_tag<http_person>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_post_req_body& local);
http_test_post_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_post_resp_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_req_body& local);
http_test_get_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_resp_body>&, const aquarius::json::value& jv);

using http_test_post_request = aquarius::virgo::http_request<"/login/new", aquarius::virgo::http_method::post, http_test_post_req_body>;
using http_test_post_response = aquarius::virgo::http_response<aquarius::virgo::http_method::post, http_test_post_resp_body>;
using http_test_get_request = aquarius::virgo::http_request<"/login", aquarius::virgo::http_method::get, http_test_get_req_body>;
using http_test_get_response = aquarius::virgo::http_response<aquarius::virgo::http_method::get, http_test_get_resp_body>;
