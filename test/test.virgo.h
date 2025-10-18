#pragma once
#include <aquarius.hpp>
struct person
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

bool operator==(const person& lhs, const person& rhs);

std::ostream& operator<<(std::ostream& os, const person& other);

class login_req_header : public aquarius::serialize::tcp_serialize
{
public:
	login_req_header() = default;
	virtual ~login_req_header() = default;

	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override;

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override;

public:
	int32 uuid;
};

class login_req_body : public aquarius::serialize::tcp_serialize
{
public:
	login_req_body() = default;
	virtual ~login_req_body() = default;

	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override;

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override;

public:
	person per_req;
};

class login_resp_header : public aquarius::serialize::tcp_serialize
{
public:
	login_resp_header() = default;
	virtual ~login_resp_header() = default;

	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override;

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override;

public:
	int32 uuid;
};

class login_resp_body : public aquarius::serialize::tcp_serialize
{
public:
	login_resp_body() = default;
	virtual ~login_resp_body() = default;

	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override;

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override;

public:
	person per_resp;
};

class http_login_req_header : public aquarius::serialize::http_serialize
{
public:
	http_login_req_header() = default;
	virtual ~http_login_req_header() = default;

	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override;

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override;

public:
	int32 uuid;
};

class http_login_req_body : public aquarius::serialize::http_serialize
{
public:
	http_login_req_body() = default;
	virtual ~http_login_req_body() = default;

	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override;

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override;

public:
	person per_req;
};

class http_login_resp_header : public aquarius::serialize::http_serialize
{
public:
	http_login_resp_header() = default;
	virtual ~http_login_resp_header() = default;

	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override;

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override;

public:
	int32 uuid;
};

class http_login_resp_body : public aquarius::serialize::http_serialize
{
public:
	http_login_resp_body() = default;
	virtual ~http_login_resp_body() = default;

	virtual void serialize(aquarius::detail::flex_buffer<char>& buffer) override;

	virtual void deserialize(aquarius::detail::flex_buffer<char>& buffer) override;

public:
	person per_resp;
};
using login_request = aquarius::virgo::tcp_request<"10001", login_req_header, login_req_body>;
using login_response = aquarius::virgo::tcp_response<"10001", login_resp_header, login_resp_body>;
using http_login_request = aquarius::virgo::http_request<"/login", http_login_req_header, http_login_req_body>;
using http_login_response = aquarius::virgo::http_response<"/login", http_login_resp_header, http_login_resp_body>;
