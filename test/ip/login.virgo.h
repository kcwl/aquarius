#pragma once
#include <aquarius/serialize/serialization.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
using namespace aquarius;

class login_req_body: public aquarius::serialization
{
public:
	login_req_body();
	virtual ~login_req_body();

	login_req_body(login_req_body&&) = default;
	login_req_body& operator=(login_req_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


	string username() const;
	string& username();

	string password() const;
	string& password();

private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};
class login_resp_body: public aquarius::serialization
{
public:
	login_resp_body();
	virtual ~login_resp_body();

	login_resp_body(login_resp_body&&) = default;
	login_resp_body& operator=(login_resp_body&&) = default;
public:
	virtual void to_binary(aquarius::flex_buffer& buffer) override;

	virtual void from_binary(aquarius::flex_buffer& buffer) override;

	virtual void to_json(aquarius::flex_buffer& buffer) override;

	virtual void from_json(aquarius::flex_buffer& buffer) override;

	virtual void to_kv(aquarius::flex_buffer& buffer) override;

	virtual void from_kv(aquarius::flex_buffer& buffer) override;


private:
	struct impl;
	std::shared_ptr<impl> impl_ptr_;
};

using login_request = aquarius::tcp_request<"10000", login_req_body>;
using login_response = aquarius::tcp_response<"10000", login_resp_body>;
