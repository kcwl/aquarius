#pragma once
#include <aquarius/serialize/http_serialize.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
using namespace aquarius;

class login_req_body: public aquarius::http_serialize
{
public:
	login_req_body();
	virtual ~login_req_body();

	login_req_body(login_req_body&&) = default;
	login_req_body& operator=(login_req_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_req_body& local);
	friend login_req_body tag_invoke(const aquarius::json::value_to_tag<login_req_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


	string username() const;
	string& username();

	string password() const;
	string& password();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class login_resp_body: public aquarius::http_serialize
{
public:
	login_resp_body();
	virtual ~login_resp_body();

	login_resp_body(login_resp_body&&) = default;
	login_resp_body& operator=(login_resp_body&&) = default;
private:
	friend void tag_invoke(const aquarius::json::value_from_tag&, aquarius::json::value& jv, const login_resp_body& local);
	friend login_resp_body tag_invoke(const aquarius::json::value_to_tag<login_resp_body>&, const aquarius::json::value& jv);

public:
	virtual void serialize(aquarius::flex_buffer& buffer) override;

	virtual void deserialize(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};


using login_request = aquarius::http_request<"10000", login_req_body>;
using login_response = aquarius::http_response<login_resp_body>;
