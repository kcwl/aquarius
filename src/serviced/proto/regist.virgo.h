#pragma once
#include <aquarius/serialize/http_serialize.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
using namespace aquarius;

class regist_req_body: public aquarius::http_serialize
{
public:
	regist_req_body();
	virtual ~regist_req_body();

	regist_req_body(regist_req_body&&) = default;
	regist_req_body& operator=(regist_req_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const regist_req_body& local);
	friend regist_req_body tag_invoke(const aquarius::json::value_to_tag<regist_req_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


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
class regist_resp_body: public aquarius::http_serialize
{
public:
	regist_resp_body();
	virtual ~regist_resp_body();

	regist_resp_body(regist_resp_body&&) = default;
	regist_resp_body& operator=(regist_resp_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const regist_resp_body& local);
	friend regist_resp_body tag_invoke(const aquarius::json::value_to_tag<regist_resp_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};

class subscribe_service_req_body: public aquarius::http_serialize
{
public:
	subscribe_service_req_body();
	virtual ~subscribe_service_req_body();

	subscribe_service_req_body(subscribe_service_req_body&&) = default;
	subscribe_service_req_body& operator=(subscribe_service_req_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subscribe_service_req_body& local);
	friend subscribe_service_req_body tag_invoke(const aquarius::json::value_to_tag<subscribe_service_req_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string group() const;
	string& group();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class subscribe_service_resp_body: public aquarius::http_serialize
{
public:
	subscribe_service_resp_body();
	virtual ~subscribe_service_resp_body();

	subscribe_service_resp_body(subscribe_service_resp_body&&) = default;
	subscribe_service_resp_body& operator=(subscribe_service_resp_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subscribe_service_resp_body& local);
	friend subscribe_service_resp_body tag_invoke(const aquarius::json::value_to_tag<subscribe_service_resp_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	std::vector<uint64_t> instances() const;
	std::vector<uint64_t>& instances();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};

class broad_service_status_req_body: public aquarius::http_serialize
{
public:
	broad_service_status_req_body();
	virtual ~broad_service_status_req_body();

	broad_service_status_req_body(broad_service_status_req_body&&) = default;
	broad_service_status_req_body& operator=(broad_service_status_req_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const broad_service_status_req_body& local);
	friend broad_service_status_req_body tag_invoke(const aquarius::json::value_to_tag<broad_service_status_req_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class broad_service_status_resp_body: public aquarius::http_serialize
{
public:
	broad_service_status_resp_body();
	virtual ~broad_service_status_resp_body();

	broad_service_status_resp_body(broad_service_status_resp_body&&) = default;
	broad_service_status_resp_body& operator=(broad_service_status_resp_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const broad_service_status_resp_body& local);
	friend broad_service_status_resp_body tag_invoke(const aquarius::json::value_to_tag<broad_service_status_resp_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string group() const;
	string& group();

	uint64 host_and_port() const;
	uint64& host_and_port();

	bool healty() const;
	bool& healty();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};


using regist_request = aquarius::http_request<"9000", regist_req_body>;
using regist_response = aquarius::http_response<regist_resp_body>;
using subscribe_service_request = aquarius::http_request<"9001", subscribe_service_req_body>;
using subscribe_service_response = aquarius::http_response<subscribe_service_resp_body>;
using broad_service_status_request = aquarius::http_request<"9002", broad_service_status_req_body>;
using broad_service_status_response = aquarius::http_response<broad_service_status_resp_body>;
