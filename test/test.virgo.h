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
person tag_invoke(const aquarius::json::value_to_tag<person>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const person& local);
class login_request_body : public aquarius::tcp_binary_serialize
{
public:
	login_request_body() = default;
	virtual ~login_request_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_req;
};
using login_request = aquarius::virgo::tcp_request<"10001", aquarius::tcp_request_header, login_request_body>;

bool operator==(const login_request_body& lhs, const login_request_body& rhs);

std::ostream& operator<<(std::ostream& os, const login_request_body& other);
login_request_body tag_invoke(const aquarius::json::value_to_tag<login_request_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_request_body& local);
class login_response_body : public aquarius::tcp_binary_serialize
{
public:
	login_response_body() = default;
	virtual ~login_response_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
};
using login_response = aquarius::virgo::tcp_response<"10001", aquarius::tcp_response_header, login_response_body>;

bool operator==(const login_response_body& lhs, const login_response_body& rhs);

std::ostream& operator<<(std::ostream& os, const login_response_body& other);
login_response_body tag_invoke(const aquarius::json::value_to_tag<login_response_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_response_body& local);
class new_http_login_request_body : public aquarius::http_json_serialize
{
public:
	new_http_login_request_body() = default;
	virtual ~new_http_login_request_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
	person per_req;
};
using new_http_login_request = aquarius::virgo::http_request<"login/new", aquarius::http_request_header, new_http_login_request_body>;

bool operator==(const new_http_login_request_body& lhs, const new_http_login_request_body& rhs);

std::ostream& operator<<(std::ostream& os, const new_http_login_request_body& other);
new_http_login_request_body tag_invoke(const aquarius::json::value_to_tag<new_http_login_request_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const new_http_login_request_body& local);
class new_http_login_response_body : public aquarius::http_json_serialize
{
public:
	new_http_login_response_body() = default;
	virtual ~new_http_login_response_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
	person per_resp;
};
using new_http_login_response = aquarius::virgo::http_response<"login/new", aquarius::http_response_header, new_http_login_response_body>;

bool operator==(const new_http_login_response_body& lhs, const new_http_login_response_body& rhs);

std::ostream& operator<<(std::ostream& os, const new_http_login_response_body& other);
new_http_login_response_body tag_invoke(const aquarius::json::value_to_tag<new_http_login_response_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const new_http_login_response_body& local);
class http_test_get_request_body : public aquarius::http_kv_serialize
{
public:
	http_test_get_request_body() = default;
	virtual ~http_test_get_request_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 user;
	string passwd;
};
using http_test_get_request = aquarius::virgo::http_request<"login", aquarius::http_request_header, http_test_get_request_body>;

bool operator==(const http_test_get_request_body& lhs, const http_test_get_request_body& rhs);

std::ostream& operator<<(std::ostream& os, const http_test_get_request_body& other);
http_test_get_request_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_request_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_request_body& local);
class http_test_get_response_body : public aquarius::http_json_serialize
{
public:
	http_test_get_response_body() = default;
	virtual ~http_test_get_response_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 user;
	string passwd;
};
using http_test_get_response = aquarius::virgo::http_response<"login", aquarius::http_response_header, http_test_get_response_body>;

bool operator==(const http_test_get_response_body& lhs, const http_test_get_response_body& rhs);

std::ostream& operator<<(std::ostream& os, const http_test_get_response_body& other);
http_test_get_response_body tag_invoke(const aquarius::json::value_to_tag<http_test_get_response_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_test_get_response_body& local);
class tcp_login_request_header : public aquarius::tcp_binary_serialize
{
public:
	tcp_login_request_header() = default;
	virtual ~tcp_login_request_header() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};
class tcp_login_request_body : public aquarius::tcp_binary_serialize
{
public:
	tcp_login_request_body() = default;
	virtual ~tcp_login_request_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_req;
};
class tcp_login_response_header : public aquarius::tcp_binary_serialize
{
public:
	tcp_login_response_header() = default;
	virtual ~tcp_login_response_header() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};
class tcp_login_response_body : public aquarius::tcp_binary_serialize
{
public:
	tcp_login_response_body() = default;
	virtual ~tcp_login_response_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
};

bool operator==(const tcp_login_request_header& lhs, const tcp_login_request_header& rhs);

std::ostream& operator<<(std::ostream& os, const tcp_login_request_header& other);

bool operator==(const tcp_login_request_body& lhs, const tcp_login_request_body& rhs);

std::ostream& operator<<(std::ostream& os, const tcp_login_request_body& other);

bool operator==(const tcp_login_response_header& lhs, const tcp_login_response_header& rhs);

std::ostream& operator<<(std::ostream& os, const tcp_login_response_header& other);

bool operator==(const tcp_login_response_body& lhs, const tcp_login_response_body& rhs);

std::ostream& operator<<(std::ostream& os, const tcp_login_response_body& other);
tcp_login_request_header tag_invoke(const aquarius::json::value_to_tag<tcp_login_request_header>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const tcp_login_request_header& local);
tcp_login_request_body tag_invoke(const aquarius::json::value_to_tag<tcp_login_request_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const tcp_login_request_body& local);
tcp_login_response_header tag_invoke(const aquarius::json::value_to_tag<tcp_login_response_header>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const tcp_login_response_header& local);
tcp_login_response_body tag_invoke(const aquarius::json::value_to_tag<tcp_login_response_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const tcp_login_response_body& local);
class http_login_request_header : public aquarius::http_json_serialize
{
public:
	http_login_request_header() = default;
	virtual ~http_login_request_header() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};
class http_login_request_body : public aquarius::http_json_serialize
{
public:
	http_login_request_body() = default;
	virtual ~http_login_request_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_req;
};
class http_login_response_header : public aquarius::http_json_serialize
{
public:
	http_login_response_header() = default;
	virtual ~http_login_response_header() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 uuid;
};
class http_login_response_body : public aquarius::http_json_serialize
{
public:
	http_login_response_body() = default;
	virtual ~http_login_response_body() = default;

	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	person per_resp;
};

bool operator==(const http_login_request_header& lhs, const http_login_request_header& rhs);

std::ostream& operator<<(std::ostream& os, const http_login_request_header& other);

bool operator==(const http_login_request_body& lhs, const http_login_request_body& rhs);

std::ostream& operator<<(std::ostream& os, const http_login_request_body& other);

bool operator==(const http_login_response_header& lhs, const http_login_response_header& rhs);

std::ostream& operator<<(std::ostream& os, const http_login_response_header& other);

bool operator==(const http_login_response_body& lhs, const http_login_response_body& rhs);

std::ostream& operator<<(std::ostream& os, const http_login_response_body& other);
http_login_request_header tag_invoke(const aquarius::json::value_to_tag<http_login_request_header>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_request_header& local);
http_login_request_body tag_invoke(const aquarius::json::value_to_tag<http_login_request_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_request_body& local);
http_login_response_header tag_invoke(const aquarius::json::value_to_tag<http_login_response_header>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_response_header& local);
http_login_response_body tag_invoke(const aquarius::json::value_to_tag<http_login_response_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const http_login_response_body& local);
using login_request = aquarius::virgo::tcp_request<"10001", aquarius::tcp_request_header, login_request_body>;
using login_response = aquarius::virgo::tcp_response<"10001", aquarius::tcp_response_header, login_response_body>;
using new_http_login_request = aquarius::virgo::http_request<"login/new", aquarius::http_request_header, new_http_login_request_body>;
using new_http_login_response = aquarius::virgo::http_response<"login/new", aquarius::http_response_header, new_http_login_response_body>;
using http_test_get_request = aquarius::virgo::http_request<"login", aquarius::http_request_header, http_test_get_request_body>;
using http_test_get_response = aquarius::virgo::http_response<"login", aquarius::http_response_header, http_test_get_response_body>;
using tcp_login_request = aquarius::virgo::tcp_request<"10002", tcp_login_request_header, tcp_login_request_body>;
using tcp_login_response = aquarius::virgo::tcp_response<"10002", tcp_login_response_header, tcp_login_response_body>;
using http_login_request = aquarius::virgo::http_request<"login", http_login_request_header, http_login_request_body>;
using http_login_response = aquarius::virgo::http_response<"login", http_login_response_header, http_login_response_body>;
