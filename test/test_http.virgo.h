#pragma once
#include <aquarius/serialize/http_serialize.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
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

class http_test_post_req_body : public aquarius::http_serialize
{
public:
	http_test_post_req_body();
	virtual ~http_test_post_req_body() = default;

	http_test_post_req_body(http_test_post_req_body&&) = default;
	http_test_post_req_body& operator=(http_test_post_req_body&&) = default;

private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv,
						   const http_test_post_req_body& local);
	friend http_test_post_req_body tag_invoke(const aquarius::json::value_to_tag<http_test_post_req_body>&,
											  const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

	int32 uuid() const;
	int32& uuid();

	http_person per_req() const;
	http_person& per_req();

private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};
class http_test_post_resp_body : public aquarius::http_serialize
{
public:
	http_test_post_resp_body();
	virtual ~http_test_post_resp_body() = default;

	http_test_post_resp_body(http_test_post_resp_body&&) = default;
	http_test_post_resp_body& operator=(http_test_post_resp_body&&) = default;

private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv,
						   const http_test_post_resp_body& local);
	friend http_test_post_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_post_resp_body>&,
											   const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

	int32 uuid() const;
	int32& uuid();

	http_person per_resp() const;
	http_person& per_resp();

private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};

class http_test_get_req_body : public aquarius::http_serialize
{
public:
	http_test_get_req_body();
	virtual ~http_test_get_req_body() = default;

	http_test_get_req_body(http_test_get_req_body&&) = default;
	http_test_get_req_body& operator=(http_test_get_req_body&&) = default;

private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv,
						   const http_test_get_req_body& local);
	friend http_test_get_req_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_req_body>&,
											 const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

	int32 user() const;
	int32& user();

	string passwd() const;
	string& passwd();

private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};
class http_test_get_resp_body : public aquarius::http_serialize
{
public:
	http_test_get_resp_body();
	virtual ~http_test_get_resp_body() = default;

	http_test_get_resp_body(http_test_get_resp_body&&) = default;
	http_test_get_resp_body& operator=(http_test_get_resp_body&&) = default;

private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv,
						   const http_test_get_resp_body& local);
	friend http_test_get_resp_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_resp_body>&,
											  const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

	int32 user() const;
	int32& user();

	string passwd() const;
	string& passwd();

private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_person& local);
http_person tag_invoke(const aquarius::json::value_to_tag<http_person>&, const aquarius::json::value& jv);

using http_test_post_http_request = aquarius::http_request<"/login/new", http_test_post_req_body>;
using http_test_post_http_response = aquarius::http_response<http_test_post_resp_body>;
using http_test_get_http_request = aquarius::http_request<"/login", http_test_get_req_body>;
using http_test_get_http_response = aquarius::http_response<http_test_get_resp_body>;
