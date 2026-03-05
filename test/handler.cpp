#define BOOST_TEST_NO_MAIN
#include "ctx_handler.hpp"
#include <aquarius/ip/http/http_server.hpp>
#include <aquarius/ip/tcp/tcp_server.hpp>
#include <boost/test/unit_test.hpp>

using namespace std::chrono_literals;

BOOST_AUTO_TEST_SUITE(test_handler_suite)

class test_handler : public aquarius::handler<test_request, test_response>
{
public:
public:
    using base_type = aquarius::handler<test_request, test_response>;

public:
    test_handler() : base_type("__handler_test_handler") {}
    virtual auto handle()
        -> aquarius::awaitable<aquarius::error_code> override
    {
        this->request() = this->response();

        co_return error_code{};
    }
};

BOOST_AUTO_TEST_CASE(ctor)
{
    test_handler test{};

    BOOST_TEST(test.name() == "__handler_test_handler");
    BOOST_TEST(!test.request());
    BOOST_CHECK(test.response() == test_response{});
}

BOOST_AUTO_TEST_CASE(test_handler_visitor_with_no_regist)
{
    error_code ec{};

    test_handler test{};

    test.visit(ec);

    BOOST_TEST(ec == aquarius::error::eof);
}

BOOST_AUTO_TEST_CASE(test_handler_visitor_with_regist)
{
    error_code ec{};

    test_handler test{};

    test.visit(ec);

    BOOST_TEST(!ec);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()