#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include "test.virgo.h"

BOOST_AUTO_TEST_SUITE(protocol)

BOOST_AUTO_TEST_CASE(ctor)
{
	{
		auto req = std::make_shared<login_tcp_request>();
		req->header().uuid_ = 1;
		req->body().per_req.sex = true;
		req->body().per_req.addr = 2;

		auto req_move = std::move(req);

		BOOST_TEST(req_move->header().uuid_ == 1);
		BOOST_TEST(req_move->body().per_req.sex == true);
		BOOST_TEST(req_move->body().per_req.addr == 2u);

		BOOST_TEST(!req);
	}

	{
		auto resp = std::make_shared<login_tcp_response>();
		resp->header().uuid_ = 1;
		resp->body().per_resp.sex = true;
		resp->body().per_resp.addr = 2;

		auto req_move = std::move(resp);

		BOOST_TEST(req_move->header().uuid_ == 1);
		BOOST_TEST(req_move->body().per_resp.sex == true);
		BOOST_TEST(req_move->body().per_resp.addr == 2u);

		BOOST_TEST(!resp);
	}

	//{
	//	auto req = std::make_shared<login_http_request>();
	//	req->header().uuid_ = 1;
	//	req->body().per_req.sex = true;
	//	req->body().per_req.addr = 2;

	//	auto req_move = std::move(req);

	//	BOOST_TEST(req_move->header().uuid_ == 1);
	//	BOOST_TEST(req_move->body().per_req.sex == true);
	//	BOOST_TEST(req_move->body().per_req.addr == 2u);

	//	BOOST_TEST(!req);
	//}

	//{
	//	auto resp = std::make_shared<login_http_response>();
	//	resp->header().uuid_ = 1;
	//	resp->body().per_resp.sex = true;
	//	resp->body().per_resp.addr = 2;

	//	auto req_move = std::move(resp);

	//	BOOST_TEST(req_move->header().uuid_ == 1);
	//	BOOST_TEST(req_move->body().per_resp.sex == true);
	//	BOOST_TEST(req_move->body().per_resp.addr == 2u);

	//	BOOST_TEST(!resp);
	//}
}

BOOST_AUTO_TEST_CASE(tcp)
{
	//{
	//	aquarius::flex_buffer buffer;

	//	auto req = std::make_shared<login_tcp_request>();
	//	req->header().uuid_ = 1;
	//	req->body().per_req.sex = true;
	//	req->body().per_req.addr = 2;
	//	req->body().per_req.age = 15;
	//	req->body().per_req.telephone = 15230214856;
	//	req->body().per_req.score = 100;
	//	req->body().per_req.hp = 200;
	//	req->body().per_req.mana = 300;
	//	req->body().per_req.name = "John";
	//	req->body().per_req.orders = std::vector<char>{ 1, 2, 3, 4, 5 };

	//	req->commit(buffer);

	//	auto req_back = std::make_shared<login_tcp_request>();
	//	req_back->consume(buffer);

	//	BOOST_CHECK_EQUAL(*req, *req_back);
	//}

	//{
	//	aquarius::flex_buffer buffer;

	//	auto resp = std::make_shared<login_tcp_response>();
	//	resp->header().uuid_ = 1;
	//	resp->body().per_resp.sex = true;
	//	resp->body().per_resp.addr = 2;
	//	resp->body().per_resp.age = 15;
	//	resp->body().per_resp.telephone = 15230214856;
	//	resp->body().per_resp.score = 100;
	//	resp->body().per_resp.hp = 200;
	//	resp->body().per_resp.mana = 300;
	//	resp->body().per_resp.name = "John";
	//	resp->body().per_resp.orders = std::vector<char>{ 1, 2, 3, 4, 5 };

	//	resp->commit(buffer);

	//	auto resp_back = std::make_shared<login_tcp_response>();
	//	resp_back->consume(buffer);

	//	BOOST_TEST(*resp == *resp_back);
	//}

	//{
	//	aquarius::flex_buffer buffer;

	//	auto req = std::make_shared<login_tcp_request>();
	//	req->header().uuid_ = 1;
	//	req->body().per_req.sex = true;
	//	req->body().per_req.addr = 2;
	//	req->body().per_req.age = 15;
	//	req->body().per_req.telephone = 15230214856;
	//	req->body().per_req.score = 100;
	//	req->body().per_req.hp = 200;
	//	req->body().per_req.mana = 300;
	//	req->body().per_req.name = "John";
	//	req->body().per_req.orders = std::vector<char>{ 1, 2, 3, 4, 5 };

	//	req->commit(buffer);

	//	auto req_back = std::make_shared<login_tcp_request>();
	//	req_back->consume(buffer);

	//	BOOST_CHECK_EQUAL(*req, *req_back);
	//}

	//{
	//	aquarius::flex_buffer buffer;

	//	auto resp = std::make_shared<login_tcp_response>();
	//	resp->header().uuid_ = 1;
	//	resp->header().result_ = 2;
	//	resp->body().per_resp.sex = true;
	//	resp->body().per_resp.addr = 2;
	//	resp->body().per_resp.age = 15;
	//	resp->body().per_resp.telephone = 15230214856;
	//	resp->body().per_resp.score = 100;
	//	resp->body().per_resp.hp = 200;
	//	resp->body().per_resp.mana = 300;
	//	resp->body().per_resp.name = "John";
	//	resp->body().per_resp.orders = std::vector<char>{ 1, 2, 3, 4, 5 };

	//	resp->commit(buffer);

	//	auto resp_back = std::make_shared<login_tcp_response>();
	//	resp_back->consume(buffer);

	//	BOOST_TEST(*resp == *resp_back);
	//}
}
BOOST_AUTO_TEST_CASE(http_protocol)
{
	//{
	//	aquarius::flex_buffer buffer;

	//	auto req = std::make_shared<http_login_request>();
	//	req->set_field("Content-Type", "aquarius-json");
	//	req->header().uuid_ = 1;
	//	req->body().per_req.sex = true;
	//	req->body().per_req.addr = 2;
	//	req->body().per_req.age = 15;
	//	req->body().per_req.telephone = 15230214856;
	//	req->body().per_req.score = 100;
	//	req->body().per_req.hp = 200;
	//	req->body().per_req.mana = 300;
	//	req->body().per_req.name = "John";
	//	req->body().per_req.orders = std::vector<char>{ 1, 2, 3, 4, 5 };

	//	req->commit(buffer);

	//	auto req_back = std::make_shared<http_login_request>();
	//	req_back->set_field("Content-Type", "aquarius-json");
	//	req_back->consume(buffer);

	//	BOOST_TEST(*req == *req_back);
	//}
	//{
	//	aquarius::flex_buffer buffer;

	//	auto req = std::make_shared<new_http_login_http_request>();
	//	req->version(aquarius::virgo::http_version::http1_1);
	//	req->body().per_req.sex = true;
	//	req->body().per_req.addr = 2;
	//	req->body().per_req.age = 15;
	//	req->body().per_req.telephone = 15230214856;
	//	req->body().per_req.score = 100;
	//	req->body().per_req.hp = 200;
	//	req->body().per_req.mana = 300;
	//	req->body().per_req.name = "John";
	//	req->body().per_req.orders = std::vector<char>{ 1, 2, 3, 4, 5 };

	//	req->commit(buffer);

	//	auto req_back = std::make_shared<new_http_login_http_request>();
	//	req_back->consume(buffer);

	//	BOOST_TEST(*req == *req_back);
	//}

	//{
	//	aquarius::flex_buffer buffer;

	//	auto resp = std::make_shared<http_login_http_response>();
	//	resp->set_field("Content-Type", "aquarius-json");
	//	resp->header().uuid_ = 1;
	//	resp->body().per_resp.sex = true;
	//	resp->body().per_resp.addr = 2;
	//	resp->body().per_resp.age = 15;
	//	resp->body().per_resp.telephone = 15230214856;
	//	resp->body().per_resp.score = 100;
	//	resp->body().per_resp.hp = 200;
	//	resp->body().per_resp.mana = 300;
	//	resp->body().per_resp.name = "John";
	//	resp->body().per_resp.orders = { 1, 2, 3, 4, 5 };

	//	resp->commit(buffer);

	//	auto resp_back = std::make_shared<http_login_response>();
	//	resp_back->set_field("Content-Type", "aquarius-json");
	//	resp_back->consume(buffer);

	//	BOOST_TEST(*resp == *resp_back);
	//}

	//{
	//	aquarius::flex_buffer buffer;

	//	auto resp = std::make_shared<new_http_login_response>();
	//	resp->set_field("Content-Type", "aquarius-json");
	//	//resp->header().uuid_ = 1;
	//	resp->body().per_resp.sex = true;
	//	resp->body().per_resp.addr = 2;
	//	resp->body().per_resp.age = 15;
	//	resp->body().per_resp.telephone = 15230214856;
	//	resp->body().per_resp.score = 100;
	//	resp->body().per_resp.hp = 200;
	//	resp->body().per_resp.mana = 300;
	//	resp->body().per_resp.name = "John";
	//	resp->body().per_resp.orders = { 1, 2, 3, 4, 5 };

	//	resp->commit(buffer);

	//	auto resp_back = std::make_shared<new_http_login_response>();
	//	resp_back->set_field("Content-Type", "aquarius-json");
	//	resp_back->consume(buffer);

	//	BOOST_TEST(*resp == *resp_back);
	//}
}

BOOST_AUTO_TEST_CASE(stream)
{
	{
		aquarius::tcp_request_header header{};

		std::cout << header;

		BOOST_TEST(!std::cout.eof());
	}
	
	{
		aquarius::tcp_response_header header{};

		std::cout << header;

		BOOST_TEST(!std::cout.eof());
	}

	{
		aquarius::http_request_header header{};

		std::cout << header;

		BOOST_TEST(!std::cout.eof());
	}
	
	{
		aquarius::http_response_header header{};

		std::cout << header;

		BOOST_TEST(!std::cout.eof());
	}
}

BOOST_AUTO_TEST_SUITE_END()