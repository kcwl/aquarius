#pragma once
#include <aquarius/serialize/http_serialize.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
using namespace aquarius;

class shake_req_body: public aquarius::http_serialize
{
public:
	shake_req_body();
	virtual ~shake_req_body();

	shake_req_body(shake_req_body&&) = default;
	shake_req_body& operator=(shake_req_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const shake_req_body& local);
	friend shake_req_body tag_invoke(const aquarius::json::value_to_tag<shake_req_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class shake_resp_body: public aquarius::http_serialize
{
public:
	shake_resp_body();
	virtual ~shake_resp_body();

	shake_resp_body(shake_resp_body&&) = default;
	shake_resp_body& operator=(shake_resp_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const shake_resp_body& local);
	friend shake_resp_body tag_invoke(const aquarius::json::value_to_tag<shake_resp_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	std::vector<string> topics() const;
	std::vector<string>& topics();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};


using shake_request = aquarius::http_request<"8000", shake_req_body>;
using shake_response = aquarius::http_response<shake_resp_body>;
