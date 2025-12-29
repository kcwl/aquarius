#pragma once
#include <aquarius.hpp>

class cmd_op_req_body : public aquarius::tcp_binary_serialize
{
public:
	cmd_op_req_body();
	virtual ~cmd_op_req_body() = default;

public:
	bool operator==(const cmd_op_req_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	bytes input;
};

class cmd_op_resp_body : public aquarius::tcp_binary_serialize
{
public:
	cmd_op_resp_body();
	virtual ~cmd_op_resp_body() = default;

public:
	bool operator==(const cmd_op_resp_body & other) const; 

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;

public:
	string output;
};

std::ostream& operator<<(std::ostream& os, const cmd_op_req_body& other);
std::ostream& operator<<(std::ostream& os, const cmd_op_resp_body& other);

void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const cmd_op_req_body& local);
cmd_op_req_body tag_invoke(const aquarius::json::value_to_tag<cmd_op_req_body>&, const aquarius::json::value& jv);
void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const cmd_op_resp_body& local);
cmd_op_resp_body tag_invoke(const aquarius::json::value_to_tag<cmd_op_resp_body>&, const aquarius::json::value& jv);

using cmd_op_tcp_request = aquarius::virgo::tcp_request<"10001", aquarius::tcp_request_header, cmd_op_req_body>;
using cmd_op_tcp_response = aquarius::virgo::tcp_response<aquarius::tcp_response_header, cmd_op_resp_body>;
