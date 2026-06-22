#pragma once
#include <aquarius/serialize/http_serialize.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
using namespace aquarius;

class cmd_op_req_body: public aquarius::http_serialize
{
public:
	cmd_op_req_body();
	virtual ~cmd_op_req_body();

	cmd_op_req_body(cmd_op_req_body&&) = default;
	cmd_op_req_body& operator=(cmd_op_req_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const cmd_op_req_body& local);
	friend cmd_op_req_body tag_invoke(const aquarius::json::value_to_tag<cmd_op_req_body>&, const aquarius::json::value& jv);

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
class cmd_op_resp_body: public aquarius::http_serialize
{
public:
	cmd_op_resp_body();
	virtual ~cmd_op_resp_body();

	cmd_op_resp_body(cmd_op_resp_body&&) = default;
	cmd_op_resp_body& operator=(cmd_op_resp_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const cmd_op_resp_body& local);
	friend cmd_op_resp_body tag_invoke(const aquarius::json::value_to_tag<cmd_op_resp_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string output() const;
	string& output();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};


using cmd_op_request = aquarius::http_request<"9200", cmd_op_req_body>;
using cmd_op_response = aquarius::http_response<cmd_op_resp_body>;
