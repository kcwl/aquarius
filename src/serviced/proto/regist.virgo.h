#pragma once
#include <aquarius.hpp>

class regist_req_body : public aquarius::tcp_binary_serialize
{
public:
	regist_req_body();
	virtual ~regist_req_body() = default;

public:
	bool operator==(const regist_req_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	int32 protocol;
	string ip_addr;
	int32 port;
};

class regist_resp_body : public aquarius::tcp_binary_serialize
{
public:
	regist_resp_body() = default;
	virtual ~regist_resp_body() = default;

public:
	bool operator==(const regist_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
};

std::ostream& operator<<(std::ostream& os, const regist_req_body& other);
std::ostream& operator<<(std::ostream& os, const regist_resp_body& other);

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const regist_req_body& local);
regist_req_body tag_invoke(const aquarius::json::value_to_tag<regist_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const regist_resp_body& local);
regist_resp_body tag_invoke(const aquarius::json::value_to_tag<regist_resp_body>&, const aquarius::json::value& jv);

using regist_tcp_request = aquarius::virgo::tcp_request<"11000", aquarius::tcp_request_header, regist_req_body>;
using regist_tcp_response = aquarius::virgo::tcp_response<aquarius::tcp_response_header, regist_resp_body>;
