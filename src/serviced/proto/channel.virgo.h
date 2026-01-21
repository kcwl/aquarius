#pragma once
#include <aquarius.hpp>

class subscribe_req_body : public aquarius::tcp_binary_serialize
{
public:
	subscribe_req_body();
	virtual ~subscribe_req_body() = default;

public:
	bool operator==(const subscribe_req_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	string topic;
	string payload;
};

class subscribe_resp_body : public aquarius::tcp_binary_serialize
{
public:
	subscribe_resp_body() = default;
	virtual ~subscribe_resp_body() = default;

public:
	bool operator==(const subscribe_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
};

class transfer_req_body : public aquarius::tcp_binary_serialize
{
public:
	transfer_req_body();
	virtual ~transfer_req_body() = default;

public:
	bool operator==(const transfer_req_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	string topic;
	bytes feedbuf;
};

class transfer_resp_body : public aquarius::tcp_binary_serialize
{
public:
	transfer_resp_body();
	virtual ~transfer_resp_body() = default;

public:
	bool operator==(const transfer_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	bytes feedbuf;
};

std::ostream& operator<<(std::ostream& os, const subscribe_req_body& other);
std::ostream& operator<<(std::ostream& os, const subscribe_resp_body& other);
std::ostream& operator<<(std::ostream& os, const transfer_req_body& other);
std::ostream& operator<<(std::ostream& os, const transfer_resp_body& other);

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subscribe_req_body& local);
subscribe_req_body tag_invoke(const aquarius::json::value_to_tag<subscribe_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subscribe_resp_body& local);
subscribe_resp_body tag_invoke(const aquarius::json::value_to_tag<subscribe_resp_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const transfer_req_body& local);
transfer_req_body tag_invoke(const aquarius::json::value_to_tag<transfer_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const transfer_resp_body& local);
transfer_resp_body tag_invoke(const aquarius::json::value_to_tag<transfer_resp_body>&, const aquarius::json::value& jv);

using subscribe_tcp_request = aquarius::virgo::tcp_request<"10000", aquarius::tcp_request_header, subscribe_req_body>;
using subscribe_tcp_response = aquarius::virgo::tcp_response<aquarius::tcp_response_header, subscribe_resp_body>;
using transfer_tcp_request = aquarius::virgo::tcp_request<"10001", aquarius::tcp_request_header, transfer_req_body>;
using transfer_tcp_response = aquarius::virgo::tcp_response<aquarius::tcp_response_header, transfer_resp_body>;
