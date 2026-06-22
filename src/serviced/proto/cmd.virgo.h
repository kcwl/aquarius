#pragma once
#include <aquarius/serialize/tcp_serialize.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class cmd_op_req_body: public aquarius::tcp_serialize
{
public:
	cmd_op_req_body();
	virtual ~cmd_op_req_body();

	cmd_op_req_body(cmd_op_req_body&&) = default;
	cmd_op_req_body& operator=(cmd_op_req_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string command() const;
	string& command();

	std::vector<string> paramers() const;
	std::vector<string>& paramers();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class cmd_op_resp_body: public aquarius::tcp_serialize
{
public:
	cmd_op_resp_body();
	virtual ~cmd_op_resp_body();

	cmd_op_resp_body(cmd_op_resp_body&&) = default;
	cmd_op_resp_body& operator=(cmd_op_resp_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string output() const;
	string& output();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};


using cmd_op_request = aquarius::tcp_request<"9200", cmd_op_req_body>;
using cmd_op_response = aquarius::tcp_response<"9200", cmd_op_resp_body>;
