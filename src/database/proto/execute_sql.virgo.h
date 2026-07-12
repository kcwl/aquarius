#pragma once
#include <aquarius/serialize/serialization.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class query_sql_req_body: public aquarius::serialization
{
public:
	query_sql_req_body();
	virtual ~query_sql_req_body();

	query_sql_req_body(query_sql_req_body&&) = default;
	query_sql_req_body& operator=(query_sql_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	string sql() const;
	string& sql();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class query_sql_resp_body: public aquarius::serialization
{
public:
	query_sql_resp_body();
	virtual ~query_sql_resp_body();

	query_sql_resp_body(query_sql_resp_body&&) = default;
	query_sql_resp_body& operator=(query_sql_resp_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	std::vector<string> results() const;
	std::vector<string>& results();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};

class execute_sql_req_body: public aquarius::serialization
{
public:
	execute_sql_req_body();
	virtual ~execute_sql_req_body();

	execute_sql_req_body(execute_sql_req_body&&) = default;
	execute_sql_req_body& operator=(execute_sql_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	string sql() const;
	string& sql();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class execute_sql_resp_body: public aquarius::serialization
{
public:
	execute_sql_resp_body();
	virtual ~execute_sql_resp_body();

	execute_sql_resp_body(execute_sql_resp_body&&) = default;
	execute_sql_resp_body& operator=(execute_sql_resp_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	int32 affected() const;
	int32& affected();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};

using query_sql_request = aquarius::tcp_request<"7000", query_sql_req_body>;
using query_sql_response = aquarius::tcp_response<"7000", query_sql_resp_body>;
using execute_sql_request = aquarius::tcp_request<"7001", execute_sql_req_body>;
using execute_sql_response = aquarius::tcp_response<"7001", execute_sql_resp_body>;
