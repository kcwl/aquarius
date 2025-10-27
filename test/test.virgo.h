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

class new_login_req_body : public aquarius::serialize::tcp_serialize
{
public:
	new_login_req_body() = default;
	virtual ~new_login_req_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};

bool operator==(const new_login_req_body& lhs, const new_login_req_body& rhs);

std::ostream& operator<<(std::ostream& os, const new_login_req_body& other);

class new_login_resp_body : public aquarius::serialize::tcp_serialize
{
public:
	new_login_resp_body() = default;
	virtual ~new_login_resp_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
};

bool operator==(const new_login_resp_body& lhs, const new_login_resp_body& rhs);

std::ostream& operator<<(std::ostream& os, const new_login_resp_body& other);

class new_http_login_req_body : public aquarius::serialize::http_serialize
{
public:
	new_http_login_req_body() = default;
	virtual ~new_http_login_req_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};

bool operator==(const new_http_login_req_body& lhs, const new_http_login_req_body& rhs);

std::ostream& operator<<(std::ostream& os, const new_http_login_req_body& other);

class new_http_login_resp_body : public aquarius::serialize::http_serialize
{
public:
	new_http_login_resp_body() = default;
	virtual ~new_http_login_resp_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
};

bool operator==(const new_http_login_resp_body& lhs, const new_http_login_resp_body& rhs);

std::ostream& operator<<(std::ostream& os, const new_http_login_resp_body& other);

class login_req_header : public aquarius::serialize::tcp_serialize
{
public:
	login_req_header() = default;
	virtual ~login_req_header() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};

bool operator==(const login_req_header& lhs, const login_req_header& rhs);

std::ostream& operator<<(std::ostream& os, const login_req_header& other);

class login_req_body : public aquarius::serialize::tcp_serialize
{
public:
	login_req_body() = default;
	virtual ~login_req_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_req;
};

bool operator==(const login_req_body& lhs, const login_req_body& rhs);

std::ostream& operator<<(std::ostream& os, const login_req_body& other);

class login_resp_header : public aquarius::serialize::tcp_serialize
{
public:
	login_resp_header() = default;
	virtual ~login_resp_header() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};

bool operator==(const login_resp_header& lhs, const login_resp_header& rhs);

std::ostream& operator<<(std::ostream& os, const login_resp_header& other);

class login_resp_body : public aquarius::serialize::tcp_serialize
{
public:
	login_resp_body() = default;
	virtual ~login_resp_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
};

bool operator==(const login_resp_body& lhs, const login_resp_body& rhs);

std::ostream& operator<<(std::ostream& os, const login_resp_body& other);

class http_login_req_header : public aquarius::serialize::http_serialize
{
public:
	http_login_req_header() = default;
	virtual ~http_login_req_header() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};

bool operator==(const http_login_req_header& lhs, const http_login_req_header& rhs);

std::ostream& operator<<(std::ostream& os, const http_login_req_header& other);

class http_login_req_body : public aquarius::serialize::http_serialize
{
public:
	http_login_req_body() = default;
	virtual ~http_login_req_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_req;
};

bool operator==(const http_login_req_body& lhs, const http_login_req_body& rhs);

std::ostream& operator<<(std::ostream& os, const http_login_req_body& other);

class http_login_resp_header : public aquarius::serialize::http_serialize
{
public:
	http_login_resp_header() = default;
	virtual ~http_login_resp_header() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};

bool operator==(const http_login_resp_header& lhs, const http_login_resp_header& rhs);

std::ostream& operator<<(std::ostream& os, const http_login_resp_header& other);

class http_login_resp_body : public aquarius::serialize::http_serialize
{
public:
	http_login_resp_body() = default;
	virtual ~http_login_resp_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
};

bool operator==(const http_login_resp_body& lhs, const http_login_resp_body& rhs);

std::ostream& operator<<(std::ostream& os, const http_login_resp_body& other);
using new_login_request = aquarius::virgo::tcp_request<"10001", aquarius::tcp_request_header, new_login_req_body>;
using new_login_response = aquarius::virgo::tcp_response<"10001", aquarius::tcp_response_header, new_login_resp_body>;
using new_http_login_request = aquarius::virgo::http_request<aquarius::virgo::http_method::post,"login", aquarius::http_request_header, new_http_login_req_body>;
using new_http_login_response = aquarius::virgo::http_response<aquarius::virgo::http_method::post,"login", aquarius::http_response_header, new_http_login_resp_body>;
using login_request = aquarius::virgo::tcp_request<"10001", login_req_header, login_req_body>;
using login_response = aquarius::virgo::tcp_response<"10001", login_resp_header, login_resp_body>;
using http_login_request = aquarius::virgo::http_request<aquarius::virgo::http_method::post,"/login", http_login_req_header, http_login_req_body>;
using http_login_response = aquarius::virgo::http_response<aquarius::virgo::http_method::post,"/login", http_login_resp_header, http_login_resp_body>;
