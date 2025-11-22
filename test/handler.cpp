#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include "ctx_handler.hpp"

BOOST_AUTO_TEST_SUITE(handlers)

BOOST_AUTO_TEST_CASE(http_handler)
{
	auto h = std::make_shared<ctx_test_http_hander>();

	aquarius::io_context io{};

	auto session = std::make_shared<aquarius::http_server_session>(io);

	auto req_ptr = std::make_shared<new_http_login_http_request>();
	req_ptr->version(aquarius::virgo::http_version::http1_1);
	req_ptr->set_field("Content-Type", "json");
	req_ptr->set_field("Server", "Aquarius 0.10.0");
	req_ptr->set_field("Connection", "keep-alive");
	req_ptr->set_field("Access-Control-Allow-Origin", "*");

	aquarius::co_spawn(io, [&]->aquarius::awaitable<void>
					   {
						   co_await h->visit(session, req_ptr);

						   BOOST_TEST(*req_ptr == *h->request());

						   BOOST_TEST(h->name() == "__handler_ctx_test_http_hander");

						   auto& response = h->response();

						   BOOST_TEST(response.version() == req_ptr->version());
						   BOOST_TEST(response.find("Content-Type") == "json");
						   BOOST_TEST(response.find("Server") == "Aquarius 0.10.0");
						   BOOST_TEST(response.find("Connection") == "keep-alive");
						   BOOST_TEST(response.find("Access-Control-Allow-Origin") == "*");
						   BOOST_TEST(static_cast<int>(response.result()) == static_cast<int>(aquarius::virgo::http_status::ok));
					   }, aquarius::detached);

	io.run();
}

BOOST_AUTO_TEST_CASE(tcp_handler)
{
	auto h = std::make_shared<ctx_tcp_test>();
	aquarius::io_context io{};
	auto session = std::make_shared<aquarius::tcp_server_session>(io);
	std::shared_ptr<login_tcp_request> req_ptr{};

	aquarius::co_spawn(io, [&]->aquarius::awaitable<void> 
					   {
						   co_await h->visit(session, req_ptr);

						   BOOST_TEST(h->name() == "__handler_ctx_tcp_test_hander");

						   BOOST_TEST(h->response().result() == aquarius::error_code{}.value());
					   }, aquarius::detached);
	io.run();
}

BOOST_AUTO_TEST_SUITE_END()