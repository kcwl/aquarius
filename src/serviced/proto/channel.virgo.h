#pragma once
#include <aquarius/serialize/tcp_serialize.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class subs_list_req_body: public aquarius::tcp_serialize
{
public:
	subs_list_req_body();
	virtual ~subs_list_req_body();

	subs_list_req_body(subs_list_req_body&&) = default;
	subs_list_req_body& operator=(subs_list_req_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class subs_list_resp_body: public aquarius::tcp_serialize
{
public:
	subs_list_resp_body();
	virtual ~subs_list_resp_body();

	subs_list_resp_body(subs_list_resp_body&&) = default;
	subs_list_resp_body& operator=(subs_list_resp_body&&) = default;

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	std::vector<string> keys() const;
	std::vector<string>& keys();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};


using subs_list_tcp_request = aquarius::tcp_request<"9100", subs_list_req_body>;
using subs_list_tcp_response = aquarius::tcp_response<"9100", subs_list_resp_body>;
