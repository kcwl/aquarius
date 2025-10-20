#pragma once
#include <boost/test/unit_test_suite.hpp>
#include <aquarius/virgo/tcp_request.hpp>
#include <aquarius/virgo/tcp_response.hpp>
#include <aquarius/virgo/http_request.hpp>
#include <aquarius/virgo/http_response.hpp>
#include "test.virgo.h"

BOOST_AUTO_TEST_CASE(tcp_protocol)
{
	{
		aquarius::flex_buffer<char> buffer;

		auto req = std::make_shared<login_request>();
		req->header().uuid = 1;
		req->body().per_req.sex = true;
		req->body().per_req.addr = 2;
		req->body().per_req.age = 15;
		req->body().per_req.telephone = 15230214856;
		req->body().per_req.score = 100;
		req->body().per_req.hp = 200;
		req->body().per_req.mana = 300;
		req->body().per_req.name = "John";
		req->body().per_req.orders = std::vector<char>{ 1, 2, 3, 4, 5 };

		req->commit(buffer);

		auto req_back = std::make_shared<login_request>();
		req_back->consume(buffer);

		BOOST_CHECK_EQUAL(*req, *req_back);
	}

	{
		aquarius::flex_buffer<char> buffer;

		auto resp = std::make_shared<login_response>();
		resp->header().uuid = 1;
		resp->body().per_resp.sex = true;
		resp->body().per_resp.addr = 2;
		resp->body().per_resp.age = 15;
		resp->body().per_resp.telephone = 15230214856;
		resp->body().per_resp.score = 100;
		resp->body().per_resp.hp = 200;
		resp->body().per_resp.mana = 300;
		resp->body().per_resp.name = "John";
		resp->body().per_resp.orders = std::vector < char>{ 1, 2, 3, 4, 5 };

		resp->commit(buffer);

		auto resp_back = std::make_shared<login_response>();
		resp_back->consume(buffer);

		BOOST_CHECK_EQUAL(*resp, *resp_back);
	}
}
BOOST_AUTO_TEST_CASE(http_protocol)
{
	{
		aquarius::flex_buffer<char> buffer;

		auto req = std::make_shared<http_login_request>();
		req->header().uuid = 1;
		req->body().per_req.sex = true;
		req->body().per_req.addr = 2;
		req->body().per_req.age = 15;
		req->body().per_req.telephone = 15230214856;
		req->body().per_req.score = 100;
		req->body().per_req.hp = 200;
		req->body().per_req.mana = 300;
		req->body().per_req.name = "John";
		req->body().per_req.orders = std::vector<char>{ 1, 2, 3, 4, 5 };

		req->commit(buffer);

		auto req_back = std::make_shared<http_login_request>();
		req_back->consume(buffer);

		BOOST_CHECK_EQUAL(*req, *req_back);
	}

	{
		aquarius::flex_buffer<char> buffer;

		auto resp = std::make_shared<http_login_response>();
		resp->header().uuid = 1;
		resp->body().per_resp.sex = true;
		resp->body().per_resp.addr = 2;
		resp->body().per_resp.age = 15;
		resp->body().per_resp.telephone = 15230214856;
		resp->body().per_resp.score = 100;
		resp->body().per_resp.hp = 200;
		resp->body().per_resp.mana = 300;
		resp->body().per_resp.name = "John";
		resp->body().per_resp.orders = { 1, 2, 3, 4, 5 };

		resp->commit(buffer);

		auto resp_back = std::make_shared<http_login_response>();
		resp_back->consume(buffer);

		BOOST_CHECK_EQUAL(*resp, *resp_back);
	}
}