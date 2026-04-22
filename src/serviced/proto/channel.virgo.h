#pragma once
#include <aquarius/serialize/tcp_serialize.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class subscribe_req_body: public aquarius::tcp_serialize
{
public:
	subscribe_req_body();
	virtual ~subscribe_req_body() = default;

	subscribe_req_body(subscribe_req_body&&) = default;
	subscribe_req_body& operator=(subscribe_req_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string topic() const;
	string& topic();

private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};
class subscribe_resp_body: public aquarius::tcp_serialize
{
public:
	subscribe_resp_body() = default;
	virtual ~subscribe_resp_body() = default;

	subscribe_resp_body(subscribe_resp_body&&) = default;
	subscribe_resp_body& operator=(subscribe_resp_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::unique_ptr<impl> impl_ptr_;
};


using subscribe_tcp_request = aquarius::tcp_request<"9100", subscribe_req_body>;
using subscribe_tcp_response = aquarius::tcp_response<subscribe_resp_body>;
