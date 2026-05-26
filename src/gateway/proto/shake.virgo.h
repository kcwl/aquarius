#pragma once
#include <aquarius/serialize/tcp_serialize.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class shake_req_body: public aquarius::tcp_serialize
{
public:
	shake_req_body();
	virtual ~shake_req_body();

	shake_req_body(shake_req_body&&) = default;
	shake_req_body& operator=(shake_req_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class shake_resp_body: public aquarius::tcp_serialize
{
public:
	shake_resp_body();
	virtual ~shake_resp_body();

	shake_resp_body(shake_resp_body&&) = default;
	shake_resp_body& operator=(shake_resp_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	std::vector<string> topics() const;
	std::vector<string>& topics();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};


using shake_tcp_request = aquarius::tcp_request<"8000", shake_req_body>;
using shake_tcp_response = aquarius::tcp_response<"8000", shake_resp_body>;
