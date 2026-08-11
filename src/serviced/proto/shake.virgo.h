#pragma once
#include <aquarius/serialize/serialization.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class shake_req_body: public aquarius::serialization
{
public:
	shake_req_body();
	virtual ~shake_req_body();

	shake_req_body(shake_req_body&&) = default;
	shake_req_body& operator=(shake_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	string host() const;
	string& host();

	int32 port() const;
	int32& port();

	string name() const;
	string& name();

	string group() const;
	string& group();

	sint32 weight() const;
	sint32& weight();

	string version() const;
	string& version();

	std::vector<string> topics() const;
	std::vector<string>& topics();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class shake_resp_body: public aquarius::serialization
{
public:
	shake_resp_body();
	virtual ~shake_resp_body();

	shake_resp_body(shake_resp_body&&) = default;
	shake_resp_body& operator=(shake_resp_body&&) = default;
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

using shake_request = aquarius::tcp_request<"/shake", shake_req_body>;
using shake_response = aquarius::tcp_response<"/shake", shake_resp_body>;
