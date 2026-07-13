#pragma once
#include <aquarius/serialize/serialization.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class query_permission_req_body: public aquarius::serialization
{
public:
	query_permission_req_body();
	virtual ~query_permission_req_body();

	query_permission_req_body(query_permission_req_body&&) = default;
	query_permission_req_body& operator=(query_permission_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	sint64 user_id() const;
	sint64& user_id();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class query_permission_resp_body: public aquarius::serialization
{
public:
	query_permission_resp_body();
	virtual ~query_permission_resp_body();

	query_permission_resp_body(query_permission_resp_body&&) = default;
	query_permission_resp_body& operator=(query_permission_resp_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	std::vector<string> permissions() const;
	std::vector<string>& permissions();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};

class check_permission_req_body: public aquarius::serialization
{
public:
	check_permission_req_body();
	virtual ~check_permission_req_body();

	check_permission_req_body(check_permission_req_body&&) = default;
	check_permission_req_body& operator=(check_permission_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	sint64 user_id() const;
	sint64& user_id();

	sint64 permission() const;
	sint64& permission();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class check_permission_resp_body: public aquarius::serialization
{
public:
	check_permission_resp_body();
	virtual ~check_permission_resp_body();

	check_permission_resp_body(check_permission_resp_body&&) = default;
	check_permission_resp_body& operator=(check_permission_resp_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};

class add_permission_req_body: public aquarius::serialization
{
public:
	add_permission_req_body();
	virtual ~add_permission_req_body();

	add_permission_req_body(add_permission_req_body&&) = default;
	add_permission_req_body& operator=(add_permission_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	sint64 user_id() const;
	sint64& user_id();

	std::vector<sint64> permissions() const;
	std::vector<sint64>& permissions();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class add_permission_resp_body: public aquarius::serialization
{
public:
	add_permission_resp_body();
	virtual ~add_permission_resp_body();

	add_permission_resp_body(add_permission_resp_body&&) = default;
	add_permission_resp_body& operator=(add_permission_resp_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};

using query_permission_request = aquarius::tcp_request<"/permission/query", query_permission_req_body>;
using query_permission_response = aquarius::tcp_response<"/permission/query", query_permission_resp_body>;
using check_permission_request = aquarius::tcp_request<"/permission/check", check_permission_req_body>;
using check_permission_response = aquarius::tcp_response<"/permission/check", check_permission_resp_body>;
using add_permission_request = aquarius::tcp_request<"/permission/add", add_permission_req_body>;
using add_permission_response = aquarius::tcp_response<"/permission/add", add_permission_resp_body>;
