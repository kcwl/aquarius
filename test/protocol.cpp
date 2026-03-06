#define BOOST_TEST_NO_MAIN
#include "test.virgo.h"
#include <boost/test/unit_test.hpp>
#include "ctx_handler.hpp"

BOOST_AUTO_TEST_SUITE(test_protocol_suite)

template<typename Request>
void check_tcp_equal_req(const Request& lhs, const Request& rhs)
{
    BOOST_TEST(lhs.version() == rhs.version());
    BOOST_TEST(lhs.header().uuid() == rhs.header().uuid());
    BOOST_TEST(lhs.header().timestamp() == rhs.header().timestamp());
    BOOST_TEST(lhs.header().sequence() == rhs.header().sequence());
    check_person(lhs.body().per_req, rhs.body().per_req);
}

template<typename Response>
void check_tcp_equal_resp(const Response& lhs, const Response& rhs)
{
    BOOST_TEST(lhs.version() == rhs.version());
    BOOST_TEST(lhs.result() == rhs.result());
    BOOST_TEST(lhs.header().uuid() == rhs.header().uuid());
    BOOST_TEST(lhs.header().timestamp() == rhs.header().timestamp());
    BOOST_TEST(lhs.header().sequence() == rhs.header().sequence());
    check_person(lhs.body().per_resp, rhs.body().per_resp);
}

template<typename Request>
void check_http_equal_req(const Request& lhs, const Request& rhs)
{
    BOOST_TEST(lhs.version() == rhs.version());
    BOOST_TEST(lhs.header().keep_alive() == rhs.header().keep_alive());
    BOOST_TEST(lhs.header().content_type() == rhs.header().content_type());
    BOOST_TEST(lhs.header().sequence() == rhs.header().sequence());
    BOOST_TEST(lhs.header().content_length() == rhs.header().content_length());
    check_person(lhs.body().per_req, rhs.body().per_req);
}

template<typename Response>
void check_http_equal_resp(const Response& lhs, const Response& rhs)
{
    BOOST_TEST(lhs.version() == rhs.version());
    BOOST_TEST(lhs.result() == rhs.result());
    BOOST_TEST(lhs.header().content_type() == rhs.header().content_type());
    BOOST_TEST(lhs.header().sequence() == rhs.header().sequence());
    BOOST_TEST(lhs.header().content_length() == rhs.header().content_length());
    check_person(lhs.body().per_resp, rhs.body().per_resp);
}

BOOST_AUTO_TEST_CASE(test_tcp_request)
{
    using test_request = login_request;

    test_request req{};

    req.version(1);
    req.header().uuid(2);
    req.header().timestamp(3);
    req.header().sequence(4);
    req.body().per_req.addr = 45;
    req.body().per_req.age = 32;
    req.body().per_req.hp = 3.2f;
    req.body().per_req.mana = 7.2;
    req.body().per_req.name = "test";
    req.body().per_req.orders = {'h','e','l','l','o'};
    req.body().per_req.score = 74;
    req.body().per_req.sex = true;
    req.body().per_req.telephone = 14589675283;


    aquarius::flex_buffer buffer;
    req.commit(buffer);

    test_request req1{};
    req1.consume_header(buffer);
    req1.consume_body(buffer);

    check_tcp_equal_req(req, req1);

    test_request req2 = req;

    check_tcp_equal_req(req, req2);

    test_request req3{};
    req3 = req;

    check_tcp_equal_req(req, req3);

    test_request req4 = std::move(req);

    check_tcp_equal_req(req3, req4);

    test_request req5{};
    req5 = std::move(req4);

    check_tcp_equal_req(req5, req3);
}

BOOST_AUTO_TEST_CASE(test_tcp_response)
{
    using test_response = login_response;

    test_response resp{};

    resp.version(1);
    resp.result(111);
    resp.header().uuid(2);
    resp.header().timestamp(3);
    resp.header().sequence(4);
    resp.body().per_resp.addr = 45;
    resp.body().per_resp.age = 32;
    resp.body().per_resp.hp = 3.2f;
    resp.body().per_resp.mana = 7.2;
    resp.body().per_resp.name = "test";
    resp.body().per_resp.orders = { 'h','e','l','l','o' };
    resp.body().per_resp.score = 74;
    resp.body().per_resp.sex = true;
    resp.body().per_resp.telephone = 14589675283;

    aquarius::flex_buffer buffer;
    resp.commit(buffer);

    test_response resp1{};
    resp1.consume_header(buffer);
    resp1.consume_body(buffer);

    check_tcp_equal_resp(resp, resp1);

    test_response resp2 = resp;

    check_tcp_equal_resp(resp, resp2);

    test_response resp3{};
    resp3 = resp;

    check_tcp_equal_resp(resp, resp3);

    test_response resp4 = std::move(resp);

    check_tcp_equal_resp(resp3, resp4);

    test_response resp5{};
    resp5 = std::move(resp4);

    check_tcp_equal_resp(resp5, resp3);
}

BOOST_AUTO_TEST_CASE(test_http_request)
{
    using test_request = new_http_login_request;

    test_request req{};

    req.version(1);
    req.header().content_type("application/json");
    req.header().keep_alive(true);
    req.header().sequence(4);
    req.body().per_req.addr = 45;
    req.body().per_req.age = 32;
    req.body().per_req.hp = 3.2f;
    req.body().per_req.mana = 7.2;
    req.body().per_req.name = "test";
    req.body().per_req.orders = { 'h','e','l','l','o' };
    req.body().per_req.score = 74;
    req.body().per_req.sex = true;
    req.body().per_req.telephone = 14589675283;


    aquarius::flex_buffer buffer;
    req.commit(buffer);

    test_request req1{};
    req1.consume_header(buffer);
    req1.consume_body(buffer);

    check_http_equal_req(req, req1);

    test_request req2 = req;

    check_http_equal_req(req, req2);

    test_request req3{};
    req3 = req;

    check_http_equal_req(req, req3);

    test_request req4 = std::move(req);

    check_http_equal_req(req3, req4);

    test_request req5{};
    req5 = std::move(req4);

    check_http_equal_req(req5, req3);
}

BOOST_AUTO_TEST_CASE(test_http_response)
{
    using test_response = new_http_login_response;

    test_response resp{};

    resp.version(1);
    resp.result(200);
    resp.header().content_type("application/json");
    resp.header().keep_alive(true);
    resp.header().sequence(4);
    resp.body().per_resp.addr = 45;
    resp.body().per_resp.age = 32;
    resp.body().per_resp.hp = 3.2f;
    resp.body().per_resp.mana = 7.2;
    resp.body().per_resp.name = "test";
    resp.body().per_resp.orders = { 'h','e','l','l','o' };
    resp.body().per_resp.score = 74;
    resp.body().per_resp.sex = true;
    resp.body().per_resp.telephone = 14589675283;


    aquarius::flex_buffer buffer;
    resp.commit(buffer);

    test_response resp1{};
    resp1.consume_header(buffer);
    resp1.consume_body(buffer);

    check_http_equal_resp(resp, resp1);

    test_response resp2 = resp;

    check_http_equal_resp(resp, resp2);

    test_response resp3{};
    resp3 = resp;

    check_http_equal_resp(resp, resp3);

    test_response resp4 = std::move(resp);

    check_http_equal_resp(resp3, resp4);

    test_response resp5{};
    resp5 = std::move(resp4);

    check_http_equal_resp(resp5, resp3);
}

BOOST_AUTO_TEST_SUITE_END()