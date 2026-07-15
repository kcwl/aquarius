#pragma once
#include <aquarius/serialize/serialization.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class regist_req_body: public aquarius::serialization
{
public:
	regist_req_body();
	virtual ~regist_req_body();

	regist_req_body(regist_req_body&&) = default;
	regist_req_body& operator=(regist_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	string name() const;
	string& name();

	string host() const;
	string& host();

	int32 port() const;
	int32& port();

	bool healthy() const;
	bool& healthy();

	string group() const;
	string& group();

	int32 weight() const;
	int32& weight();

	string version() const;
	string& version();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class regist_resp_body: public aquarius::serialization
{
public:
	regist_resp_body();
	virtual ~regist_resp_body();

	regist_resp_body(regist_resp_body&&) = default;
	regist_resp_body& operator=(regist_resp_body&&) = default;
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

class publish_req_body: public aquarius::serialization
{
public:
	publish_req_body();
	virtual ~publish_req_body();

	publish_req_body(publish_req_body&&) = default;
	publish_req_body& operator=(publish_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	std::vector<string> topics() const;
	std::vector<string>& topics();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class publish_resp_body: public aquarius::serialization
{
public:
	publish_resp_body();
	virtual ~publish_resp_body();

	publish_resp_body(publish_resp_body&&) = default;
	publish_resp_body& operator=(publish_resp_body&&) = default;
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

using regist_request = aquarius::tcp_request<"9000", regist_req_body>;
using regist_response = aquarius::tcp_response<"9000", regist_resp_body>;
using publish_request = aquarius::tcp_request<"9001", publish_req_body>;
using publish_response = aquarius::tcp_response<"9001", publish_resp_body>;
