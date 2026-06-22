#pragma once
#include <aquarius/serialize/http_serialize.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
using namespace aquarius;

class subs_list_req_body: public aquarius::http_serialize
{
public:
	subs_list_req_body();
	virtual ~subs_list_req_body();

	subs_list_req_body(subs_list_req_body&&) = default;
	subs_list_req_body& operator=(subs_list_req_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subs_list_req_body& local);
	friend subs_list_req_body tag_invoke(const aquarius::json::value_to_tag<subs_list_req_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class subs_list_resp_body: public aquarius::http_serialize
{
public:
	subs_list_resp_body();
	virtual ~subs_list_resp_body();

	subs_list_resp_body(subs_list_resp_body&&) = default;
	subs_list_resp_body& operator=(subs_list_resp_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const subs_list_resp_body& local);
	friend subs_list_resp_body tag_invoke(const aquarius::json::value_to_tag<subs_list_resp_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	std::vector<string> keys() const;
	std::vector<string>& keys();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};


using subs_list_request = aquarius::http_request<"9100", subs_list_req_body>;
using subs_list_response = aquarius::http_response<subs_list_resp_body>;
