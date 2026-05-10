#pragma once
#include <aquarius/serialize/tcp_serialize.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class regist_req_body: public aquarius::tcp_serialize
{
public:
	regist_req_body();
	virtual ~regist_req_body();

	regist_req_body(regist_req_body&&) = default;
	regist_req_body& operator=(regist_req_body&&) = default;

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
	std::unique_ptr<impl> impl_ptr_;
};
class regist_resp_body: public aquarius::tcp_serialize
{
public:
	regist_resp_body();
	virtual ~regist_resp_body();

	regist_resp_body(regist_resp_body&&) = default;
	regist_resp_body& operator=(regist_resp_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};

struct instance
{
	string host;
	int32 port;
};

class subscribe_service_req_body: public aquarius::tcp_serialize
{
public:
	subscribe_service_req_body();
	virtual ~subscribe_service_req_body();

	subscribe_service_req_body(subscribe_service_req_body&&) = default;
	subscribe_service_req_body& operator=(subscribe_service_req_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string group() const;
	string& group();

private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};
class subscribe_service_resp_body: public aquarius::tcp_serialize
{
public:
	subscribe_service_resp_body();
	virtual ~subscribe_service_resp_body();

	subscribe_service_resp_body(subscribe_service_resp_body&&) = default;
	subscribe_service_resp_body& operator=(subscribe_service_resp_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	std::vector<instance> instances() const;
	std::vector<instance>& instances();

private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};

class broad_service_status_req_body: public aquarius::tcp_serialize
{
public:
	broad_service_status_req_body();
	virtual ~broad_service_status_req_body();

	broad_service_status_req_body(broad_service_status_req_body&&) = default;
	broad_service_status_req_body& operator=(broad_service_status_req_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};
class broad_service_status_resp_body: public aquarius::tcp_serialize
{
public:
	broad_service_status_resp_body();
	virtual ~broad_service_status_resp_body();

	broad_service_status_resp_body(broad_service_status_resp_body&&) = default;
	broad_service_status_resp_body& operator=(broad_service_status_resp_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string group() const;
	string& group();

	string host() const;
	string& host();

	int32 port() const;
	int32& port();

	bool healty() const;
	bool& healty();

private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};


using regist_tcp_request = aquarius::tcp_request<"9000", regist_req_body>;
using regist_tcp_response = aquarius::tcp_response<"9000", regist_resp_body>;
using subscribe_service_tcp_request = aquarius::tcp_request<"9001", subscribe_service_req_body>;
using subscribe_service_tcp_response = aquarius::tcp_response<"9001", subscribe_service_resp_body>;
using broad_service_status_tcp_request = aquarius::tcp_request<"9002", broad_service_status_req_body>;
using broad_service_status_tcp_response = aquarius::tcp_response<"9002", broad_service_status_resp_body>;
